#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <stdint.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>
#include <set>

#include <errno.h>
#include <getopt.h>

typedef long user_id;

struct message
{
    long id;
    user_id from;
    user_id to;
    long time;
	
    bool operator < (const message& rhs) const
    {
        return this->time < rhs.time;
    }
};

struct stats
{
    // The number of messages between the two users.
    int messages;
	
    // The number of messages moving to the first user.
    int left;
	
    // The number of messages moving to the second user.
    int right;
	
    // The number of conversations between the two users.
    int conversations;
	
    // The average number of messages per conversation.
    int convo_avg;
	
    // The minimum number of messages per conversation.
    int convo_min;
	
    // The maximum number of messages per conversation.
    int convo_max;
	
    // The value of tau.
    int tau;
	
    // The average balance.
    double balance;
	
    // The trust value.
    double trust;
	
    // The response time from the first user to the second user.
    double response_A;
	
    // The response time from the second user to the first user.
    double response_B;
    
    //The inversion score of A and B; how often A prefers B over other users
    double inversions_AB;
    
    //The inversion score of B and A; how often B prefers A over other users
    double inversions_BA;
};

typedef std::pair<user_id, user_id> user_pair;
typedef std::vector<message> timeline;
typedef timeline conversation;

void usage(const char* prog_name);
void create_timelines(std::ifstream& fin, std::map<user_pair, timeline>* ut, std::map<user_id, timeline>* uf);
void calculate_all_pairwise_trust(const std::map<user_pair, timeline>& ut, const std::map<user_id, timeline>& uf,
                                  const double S, const int num, std::map<user_pair, stats>* T_C,
								  std::map<user_id, std::vector<user_pair> >* inversions);
stats calculate_pairwise_trust(const std::map<user_pair, timeline>& ut, timeline tl, const std::map<user_id, timeline>& uf,
                               const double S, const int num, std::map<user_id, std::vector<user_pair> >* inversions) throw ();
int calc_tau(const timeline& tl) throw (std::logic_error);
void split_conversations(const timeline& tl, const int cutoff, const int num,
                         std::list<timeline>* tls);
double conversation_balance(const timeline& tl) throw (std::logic_error);
double response_time (const std::map<user_pair, timeline>& ut, const timeline& tl, const std::map<user_id, timeline>& uf, 
				   const user_id& A, const user_id& B, std::map<user_id, std::vector<user_pair> >* inversions, const double window);

std::map<user_id,int> friend_count;
std::map<user_id,int> conv_count;

int main(int argc, char* argv[]) throw ()
{
    double smoothing = 4.0;
    int number = 3;
	
    // Parse arguments.
    const char* prog_name = argv[0];
    int ch;
    const char* optstring = "S:n:";
    const struct option opts[] = {
        {"smoothing", required_argument, NULL, 'S'},
        {"number", required_argument, NULL, 'n'},
        {0,        0,           0,    0},
    };
	
    while ((ch = getopt_long(argc, argv, optstring, opts, NULL)) != -1)
    {
        double value;
        int value2;
        char* end;
        switch (ch)
        {
            case 'S':
                value = strtod(optarg, &end);
                if (end == optarg || *end != '\0' || errno == ERANGE)
                {
                    fprintf(stderr, "The smoothing factor "
							"must be a positive double.\n");
                    usage(prog_name);
                }
                smoothing = value;
                if (smoothing < 0)
                {
                    fprintf(stderr, "The smoothing factor "
							"must be positive.\n");
                    usage(prog_name);
                }
                break;
            case 'n':
                value2 = strtol(optarg, &end, 10);
                if (end == optarg || *end != '\0' || errno == ERANGE)
                {
                    fprintf(stderr, "The number "
							"must be a positive integer.\n");
                    usage(prog_name);
                }
                number = value2;
                if (number < 0)
                {
                    fprintf(stderr, "The number "
							"must be positive.\n");
                    usage(prog_name);
                }
                break;
            default:
                usage(prog_name);
        }
    }
    argc -= optind;
    argv += optind;
	
    if (argc != 2)
    {
        usage(prog_name);
    }
	
    std::ifstream fin(argv[0]);
    std::ofstream fout(argv[1]);
	
    if (!fin)
    {
        fprintf(stderr, "Error opening input file");
        usage(prog_name);
    }
	
    if (!fout)
    {
        fprintf(stderr, "Error opening output file");
        usage(prog_name);
    }
	
    std::map<user_pair, timeline> ut;
    std::map<user_pair, stats> T_C;
    std::map<user_id, timeline> uf;
    std::map<user_id, std::vector<user_pair> > inversions;
	
    create_timelines(fin, &ut, &uf);
    std::cout << "Loaded timelines" << std::endl;
    calculate_all_pairwise_trust(ut, uf, smoothing, number, &T_C, &inversions);
    std::cout << "Calculated pairwise trust" << std::endl;
    for (std::map<user_pair, stats>::const_iterator i = T_C.begin(); i != T_C.end(); ++i)
    {
        user_id A = i->first.first;
        user_id B = i->first.second;
        stats S = i->second;
		double iAB = 0;
		double iBA = 0;
		
		std::map<user_id,std::vector<user_pair> >::iterator k = inversions.find(A);
		if (k != inversions.end())
		{
			for (unsigned int j = 0; j < k->second.size(); j++)
			{
				if (k->second[j].first == B) iAB++;
			}
		}
		
		k = inversions.find(B);
		if (k != inversions.end())
		{
			for (unsigned int j = 0; j < k->second.size(); j++)
			{
				if (k->second[j].first == A) iBA++;
			}
		}
		S.inversions_BA = iAB/std::max(conv_count[A],1);
		S.inversions_AB = iBA/std::max(conv_count[B],1);
		
        if (S.trust == -1)
        {
            continue;
        }
        fout << A <<" "<< B <<" "<< S.messages <<" "<< S.left <<" "<< S.right
		<<" "<< S.conversations <<" "<< S.convo_avg <<" "<< S.convo_min
		<<" "<< S.convo_max <<" "<< S.tau <<" "<< S.balance <<" "<< S.trust
		<<" "<< S.response_A <<" "<< S.response_B << " " << S.inversions_AB 
		<< " " << S.inversions_BA << "\n";
    }
	
    return 0;
}

void usage(const char* prog_name)
{
    fprintf(stderr, "Usage: %s [-S | --smoothing <positive double>] \n"
			"          [-n | --number <positive integer>] \n"
			"          <input_filename> "
			"<output_filename>\n\n", prog_name);
    fprintf(stderr, "Input format:\n");
    fprintf(stderr, "<id> SP <from> SP <to> SP <unix timestamp>\n\n");
    fprintf(stderr, "Output format:\n");
    fprintf(stderr, "<user id> SP <user id> SP <trust value>\n\n");
    fprintf(stderr, "-n, --smoothing\n");
    fprintf(stderr, "    The smoothing factor.\n");
    fprintf(stderr, "-n, --number\n");
    fprintf(stderr, "    The number of messages necessary to consider a "
			"conversation.\n");
    exit(-1);
}

void create_timelines(std::ifstream& fin, std::map<user_pair, timeline>* ut, std::map<user_id, timeline>* uf)
{
    uint64_t id = 0;
	
    while (!fin.eof() && fin >> id)
    {
        message m;
        m.id = id;
        fin >> m.from;
        fin >> m.to;
        fin >> m.time;
        user_id A = std::min(m.from, m.to);
        user_id B = std::max(m.from, m.to);
        (*ut)[std::make_pair(A, B)].push_back(m);
        if (m.from == A) (*uf)[A].push_back(m);
        else (*uf)[B].push_back(m);
		friend_count[A] = 0;
		conv_count[A] = 0;
    }
}

void calculate_all_pairwise_trust(const std::map<user_pair, timeline>& ut, const std::map<user_id, timeline>& uf,
                                  const double S, const int num, std::map<user_pair, stats>* T_C, 
								  std::map<user_id, std::vector<user_pair> >* inversions)
{
    int count = 0;
    const int interval = ut.size() / 100;
	
    for (std::map<user_pair, timeline>::const_iterator i = ut.begin();
		 i != ut.end(); ++i)
    {
        if (count % interval == 0)
        {
            printf("\r%03i%% done pairwise trust", count / interval);
        }
        ++ count;
        const user_id &A = i->first.first;
        const user_id &B = i->first.second;
        const timeline tl = i->second;
        if (tl.size() >= (unsigned int) num)
        {
            (*T_C)[std::make_pair(A, B)] = calculate_pairwise_trust(ut, tl, uf, S, num, inversions);
        }
        else
		{
			response_time(ut, tl, uf, B, A, inversions,0);
			response_time(ut, tl, uf, A, B, inversions,0);
		}
    }
    std::cout << std::endl;
}

// Pass tl by value as we need to sort it, which implies a copy, or sorting the
// original.  I'm not a fan of requiring that the original be non-const.
stats calculate_pairwise_trust(const std::map<user_pair, timeline>& ut, timeline tl, 
                               const std::map<user_id, timeline>& uf, const double S, const int num,
							   std::map<user_id, std::vector<user_pair> >* inversions) throw ()
{
    if (num < 1)
    {
        throw std::logic_error("'num' should be greater than 0");
    }
    if (tl.size() < (unsigned) num)
    {
        throw std::logic_error("The timeline must be larger than 'num' messages");
    }
    stats stat;
    stat.messages = tl.size();
    stat.left = 0;
    stat.right = 0;
    user_id A = std::min(tl.front().from, tl.front().to);
    user_id B = std::max(tl.front().from, tl.front().to);
    for (timeline::const_iterator i = tl.begin(); i != tl.end(); ++i)
    {
        if (i->from == A)
        {
            ++ stat.right;
        }
        else
        {
            ++ stat.left;
        }
    }
    std::list<timeline> tls;
    int tau = 0;
    double response_A = 0;
    double response_B = 0;
    std::sort(tl.begin(), tl.end());
    tau = calc_tau(tl);
    split_conversations(tl, (int) S * tau, num, &tls);
	//std::cout << "Number of timelines " << tls.size() << std::endl;
    response_A = response_time(ut, tl, uf, A, B, inversions,S*tau);
    response_B = response_time(ut, tl, uf, B, A, inversions,S*tau);
    stat.conversations = tls.size();
    stat.tau = tau;
    stat.response_A = response_A;
    stat.response_B = response_B;
    if (tls.size() > 0)
    {
        stat.balance = 0;
        stat.trust = 0;
        stat.convo_avg = 0;
        stat.convo_min = tl.size();
        stat.convo_max = 0;
        for (std::list<timeline>::const_iterator i = tls.begin();
			 i != tls.end(); ++i)
        {
            double balance = conversation_balance(*i);
            int size = i->size();
            stat.balance += balance;
            stat.convo_avg += size;
            if (size < stat.convo_min)
            {
                stat.convo_min = size;
            }
            if (size > stat.convo_max)
            {
                stat.convo_max = size;
            }
            stat.trust += size * balance;
        }
        stat.balance /= tls.size();
        stat.convo_avg /= tls.size();
    }
    else
    {
        stat.balance = -1;
        stat.trust = -1;
        stat.convo_avg = -1;
        stat.convo_min = -1;
        stat.convo_max = -1;
    }
    return stat;
}

int calc_tau(const timeline& tl) throw (std::logic_error)
{
    timeline::const_iterator p = tl.begin();
    timeline::const_iterator q = tl.begin();
    std::list<int> deltas;
	
    for (++q; q != tl.end(); ++p, ++q)
    {
        int delta = q->time - p->time;
        if (delta < 0)
        {
            throw std::logic_error("`calc_tau` requires a sorted timeline.");
        }
        deltas.push_back(delta);
    }
	
    int sum = std::accumulate(deltas.begin(), deltas.end(), 0);
    if (sum == 0)
    {
        return 0;
    }
    return std::min((sum / deltas.size()), (unsigned long) 1209600);	// Cap tau at 2 weeks
}

void split_conversations(const timeline& tl, const int cutoff, const int num,
                         std::list<timeline>* tls)
{
    timeline::const_iterator p = tl.begin();
    timeline::const_iterator q = tl.begin();
	
    tls->clear();
    tls->push_back(timeline());
	
    for (++q; q != tl.end(); ++p, ++q)
    {
        int delta = q->time - p->time;
        if (delta < 0)
        {
            throw std::logic_error("`tau` requires a sorted timeline.");
        }
        tls->back().push_back(*p);
        if (delta > cutoff)
        {
            // Check if the conversation that just ended was of sufficient
            // length, and discard if it is not.
            if (tls->back().size() < (unsigned int) num)
            {
                tls->pop_back();
            }
            // Start a new conversation.
            tls->push_back(timeline());
        }
    }
    tls->back().push_back(*p);
    if (tls->back().size() < (unsigned int) num)
    {
        tls->pop_back();
    }
}

double conversation_balance(const timeline& tl) throw (std::logic_error)
{
    user_id A = tl.front().from;
    user_id B = tl.front().to;
    int count_A = 0;
    int count_B = 0;
    double p = 0.0;
	
    for (timeline::const_iterator i = tl.begin(); i != tl.end(); ++i)
    {
        if (i->from == A && i->to == B)
        {
            ++ count_A;
        }
        else if (i->from == B && i->to == A)
        {
            ++ count_B;
        }
        else
        {
            throw std::logic_error("conversation_balance must be between two users.");
        }
    }
    p = count_A / (double) (count_A + count_B);
	
    if (p == 0 || p == 1)
    {
        return 0;
    }
    return (0 - p * log(p)) - ((1 - p) * log(1 - p));
}

double response_time (const std::map<user_pair, timeline>& ut, const timeline& tl, const std::map<user_id, timeline>& uf, 
				   const user_id& A, const user_id& B, std::map<user_id, std::vector<user_pair> >* inversions, const double window)
{
	// Find response time for each message to B
	int response_time = 0;
	int num_messages = 0;
	int num_to_search = 100;
	int nr_messages = (uf.find(B)->second).size();
	friend_count[A]++;
    for (timeline::const_iterator p = tl.begin(); p != tl.end(); p++)
    {
		int time_sent = 0;
		// Find all messages from the time A sent B the message and the time B replied to A
		if (p->to == B) 
		{ 
			time_sent = p->time; // Sent time
			int time_responded = 0;
			int count = 1;
			for (timeline::const_iterator q = p + 1; q != tl.end(); q++)
			{
				if (q->to == B) break;
				if (q->from == B)
				{
					time_responded = q->time; // Responded time
					if (time_responded > time_sent+window) time_responded = 0;
					else conv_count[B]++;
					break;
				}
				if (count > num_to_search)
				{
					std::cout << "Response not found within " << num_to_search << " messages." << std::endl;
					break;
				}
				count++;
			}
			if (time_responded == 0) continue;
			
			// Find all messages in the interval
			int messages_before = 0;
			std::map<user_id,timeline>::const_iterator ib = uf.find(B);
			if (ib !=uf.end())
			{
				timeline bt = ib->second;
				std::sort(bt.begin(),bt.end());
				timeline::iterator e = bt.begin();
				std::set<user_id> seen_users;
				seen_users.clear();
				for (timeline::iterator k = bt.begin(); k != bt.end(); k++)
				{
					if (k->time > time_sent && k->time < time_responded)
					{ 
						e = k;
						messages_before += 1;
						if (seen_users.insert(k->to).second==true)	//only add inversion score once for one user for one exchange
							(*inversions)[B].push_back(std::make_pair(k->to,A));
					}
					//if (time_responded == 0 && k->time > time_sent)
					//{
					//	nr_messages+= 1;
					//	(*inversions)[B].push_back(std::make_pair(k->to,A));
					//}
				}
				if (messages_before > 0) bt.erase(bt.begin(), e);
			}
			if (time_responded > 0)
			{
				response_time += messages_before; // Response time
				num_messages += 1;
			}
			//else 
			//{
			//	if (nr_messages > 0) response_time += nr_messages*num_to_search;
			//}
		}
    }
    if (num_messages < 1) return response_time;
    return 1.0*response_time/nr_messages;
}
