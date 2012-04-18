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

#include <errno.h>
#include <getopt.h>
#include <stdint.h>

#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"

#include <math.h>

#define citer const_iterator

typedef uint32_t user_id;
typedef uint64_t message_id;

typedef boost::tuple<time_t, message_id, user_id> directed_message;
typedef boost::tuple<time_t, message_id> undirected_message;

struct stats
{
    // The number of @messages from A to B (directed_message)
    int m_AB;

    // The number of @messages sent to B (but not necessarily (and not
    // excluding) those sent by A).
    int m_B_in;

    // The number of messages sent by B (includes broadcasts and @ messages)
    int m_B_out;

    // The number of @messages sent by A (not necessarily (and not excluding)
    // those sent to B).
    int m_A_out;

    // The total number of propagations sent by B (could have been propagated
    // from individuals other than A).
    int prop_B;

    // The number of propagations sent by B that are from A.
    int prop_AB;

    // The propagation energy B spends on A.
    double energy;

    // How worthy B considers A's messages to be (re: propagating).
    double worthy;
    
    //The degree of A and B reciprocating propagations
    double reciprocity;
    
    //The degree similarity of A and B
    double similarity;
};

typedef std::list<directed_message> incoming_list;
typedef std::list<undirected_message> outgoing_list;
typedef std::map<user_id, incoming_list> incoming_map;
typedef std::map<user_id, outgoing_list> outgoing_map;
typedef std::map<std::pair<user_id, user_id>, stats> trust_map;

void usage(const char* prog_name);
void load_directed_messages(std::ifstream& fin, incoming_map&, outgoing_map&);
void load_undirected_messages(std::ifstream& fin, outgoing_map&);
template<typename T> void sort(std::map<user_id, std::list<T> >&);
void calculate_trust_all(const incoming_map&, const outgoing_map&, trust_map&, const time_t min, const time_t max, const int thresh, const std::map<user_id, int>&);
void calculate_trust_one(const user_id, const incoming_list&, const outgoing_list&, trust_map&, const time_t min, const time_t max, const int thresh, std::map<std::pair<user_id, user_id>, int>& propAB);
void load_followers(std::ifstream& ffol, std::map<user_id,int>&);

int main(int argc, char* argv[])
{
    time_t t_min = 1 * 60;
    time_t t_max = 120 * 60;
    int thresh = 3;

    // Parse arguments.
    const char* prog_name = argv[0];
    int ch;
    const char* optstring = "n:x:t:";
    const struct option opts[] = {
        {"min", required_argument, NULL, 'n'},
        {"max", required_argument, NULL, 'x'},
        {"thresh", required_argument, NULL, 't'},
        {0,        0,           0,    0},
    };

    while ((ch = getopt_long(argc, argv, optstring, opts, NULL)) != -1)
    {
        time_t value;
        char* end;
        switch (ch)
        {
            case 'n':
                value = strtol(optarg, &end, 10);
                if (end == optarg || *end != '\0' || errno == ERANGE || value < 0)
                {
                    fprintf(stderr, "The tau_min "
                                    "must be a positive integer.\n");
                    usage(prog_name);
                }
                t_min = value;
                break;
            case 'x':
                value = strtol(optarg, &end, 10);
                if (end == optarg || *end != '\0' || errno == ERANGE || value < 0)
                {
                    fprintf(stderr, "The tau_max "
                                    "must be a positive integer.\n");
                    usage(prog_name);
                }
                t_max = value;
                break;
            case 't':
                value = strtol(optarg, &end, 10);
                if (end == optarg || *end != '\0' || errno == ERANGE || value < 0)
                {
                    fprintf(stderr, "The threshold "
                                    "must be a positive integer.\n");
                    usage(prog_name);
                }
                thresh = value;
                break;
            default:
                usage(prog_name);
        }
    }
    argc -= optind;
    argv += optind;

    if (argc != 4)
    {
        usage(prog_name);
    }

    std::ifstream fdir(argv[0]);
    std::ifstream fbca(argv[1]);
    std::ofstream fout(argv[2]);
    std::ifstream ffol(argv[3]);

    if (!fdir)
    {
        fprintf(stderr, "Error opening directed file.\n");
        usage(prog_name);
    }

    if (!fbca)
    {
        fprintf(stderr, "Error opening broadcast file.\n");
        usage(prog_name);
    }

    if (!fout)
    {
        fprintf(stderr, "Error opening output file.\n");
        usage(prog_name);
    }
    
    if (!ffol)
    {
        fprintf(stderr, "Error opening followers file.\n");
        usage(prog_name);
    }

    incoming_map in;
    outgoing_map out;
    trust_map tmap;
    std::map<user_id, int> followers;

    load_directed_messages(fdir, in, out);
    load_undirected_messages(fbca, out);
    load_followers(ffol, followers);

    sort(in);
    sort(out);

    calculate_trust_all(in, out, tmap, t_min, t_max, thresh, followers);

    for (trust_map::citer i = tmap.begin(); i != tmap.end(); ++i)
    {
        const std::pair<user_id, user_id>& users = i->first;
        const stats& S = i->second;

        fout << users.first <<" "<< users.second
             <<" "<< S.m_AB <<" "<< S.m_B_in
             <<" "<< S.m_B_out <<" "<< S.m_A_out
             <<" "<< S.prop_B <<" "<< S.prop_AB
             <<" "<< S.energy <<" "<< S.worthy
             <<" "<< S.reciprocity <<" "<< S.similarity <<"\n";
    }
}

void usage(const char* prog_name)
{
    fprintf(stderr, "Usage: %s [-n | --min <positive integer>] \n"
                    "          [-x | --max <positive integer>] \n"
                    "          [-t | --thresh <positive integer>] \n"
                    "          <directed_messages> <broadcast_messages> "
                    "<output_filename> <senders_followers>\n\n", prog_name);
    fprintf(stderr, "Directed input format:\n");
    fprintf(stderr, "<id> SP <from> SP <to> SP <unix timestamp>\n\n");
    fprintf(stderr, "Broadcast input format:\n");
    fprintf(stderr, "<id> SP <from> SP <unix timestamp>\n\n");
    fprintf(stderr, "Output format:\n");
    fprintf(stderr, "<user id> SP <user id> SP <trust value>\n\n");
    fprintf(stderr, "Senders' followers input format:\n");
    fprintf(stderr, "<user id> SP <number of followers>\n\n");
    fprintf(stderr, "-n, --min\n");
    fprintf(stderr, "    The tau_min value in seconds\n");
    fprintf(stderr, "-x, --max\n");
    fprintf(stderr, "    The tau_max value in seconds\n");
    fprintf(stderr, "-t, --thresh\n");
    fprintf(stderr, "    The minimum number or messages propagated (< T will be dropped)\n");
    exit(-1);
}

void load_followers(std::ifstream& ffol, std::map<user_id,int>& followers)
{
   user_id id = 0;
   int userFollowers = 0;
   while (ffol >> id)
   {
     ffol >> userFollowers;
     if (ffol.eof()) {  throw std::runtime_error("Directed input should have four values per line."); }
     followers[id] = userFollowers;
   }
}

void load_directed_messages(std::ifstream& fin, incoming_map& in, outgoing_map& out)
{
    message_id id = 0;
    user_id fr = 0;
    user_id to = 0;
    time_t t = 0;

    while (fin >> id)
    {
        fin >> fr;
        fin >> to;
        fin >> t;
        if (fr == to)
        {
            continue;
        }
        if (fin.eof())
        {
            throw std::runtime_error("Directed input should have four values per line.");
        }
        in[to].push_back(boost::make_tuple(t, id, fr));
        out[fr].push_back(boost::make_tuple(t, id));
    }
}

template<typename T> void sort(std::map<user_id, std::list<T> >& map)
{
    for (typename std::map<user_id, std::list<T> >::iterator i = map.begin(); i != map.end(); ++i)
    {
        i->second.sort();
    }
}

void load_undirected_messages(std::ifstream& fin, outgoing_map& out)
{
    message_id id = 0;
    user_id fr = 0;
    time_t t = 0;

    while (fin >> id)
    {
        fin >> fr;
        fin >> t;
        if (fin.eof())
        {
            throw std::runtime_error("Undirected input should have three values per line.");
        }
        out[fr].push_back(boost::make_tuple(t, id));
    }
}

double H(double x)
{
	return (-1*x*log(x)-(1-x)*log(1-x));
}

void calculate_trust_all(const incoming_map& in, const outgoing_map& out, trust_map& tmap, const time_t min, 
			 const time_t max, const int thresh, const std::map<user_id, int>& followers)
{
	std::map<std::pair<user_id, user_id>, int> propAB;
    for (incoming_map::citer i = in.begin(); i != in.end(); ++i)
    {
        outgoing_map::citer j = out.find(i->first);
        if (j == out.end())
        {
            continue;
        }
        calculate_trust_one(i->first, i->second, j->second, tmap, min, max, thresh, propAB);
    }
    for (trust_map::iterator i = tmap.begin(); i != tmap.end(); ++i)
    {
        const user_id& A = i->first.first;
        const user_id& B = i->first.second;
        outgoing_map::citer a = out.find(A);
        if (a == out.end())
        {
            i->second.m_A_out = 0;
        }
        else
        {
            i->second.m_A_out = a->second.size();
        }
        
        int pAB = 0;
        int pBA = 0;
        std::map<std::pair<user_id, user_id>, int>::iterator p = propAB.find(std::make_pair(A,B));
        if (p != propAB.end()) { pAB = p->second; }
        p = propAB.find(std::make_pair(B,A));
        if (p != propAB.end()) { pBA = p->second; }
        if (pAB > 0 && pBA > 0) i->second.reciprocity = H((double)pAB/(double)(pAB+pBA));
        else i->second.reciprocity = 0;
	
	int followersA = 0;
	int followersB = 0;
	std::map<user_id, int>::const_iterator f = followers.find(A);
	if (f != followers.end()) { followersA = f->second; }
	f = followers.find(B);
	if (f != followers.end()) { followersB = f->second; }
	if (followersA > 0 && followersB > 0) i->second.similarity = H((double)followersA/(double)(followersA+followersB));
	else i->second.similarity = 0;
    }
}

void calculate_trust_one(const user_id uid, const incoming_list& in, const outgoing_list& out, trust_map& tmap, const time_t min, const time_t max, const int thresh, std::map<std::pair<user_id, user_id>, int>& propAB)
{
    incoming_list::citer i = in.begin();
    outgoing_list::citer o = out.begin();

    int prop_b = 0;
    std::map<user_id, int> m_ab;
    std::map<user_id, int> prop_ab;
    while (i != in.end())
    {
        // Number of messages from A to B.
        ++ m_ab[boost::get<2>(*i)];

        // One can show that for any `i_x` all values of `o` less than `i_x + min`
        // will be less than `i_y + min` for all `i_y > i_x`.
        while (o != out.end() && boost::get<0>(*o) < boost::get<0>(*i) + min)
        {
        	++ o;
        }
        if (o != out.end() && boost::get<0>(*o) < boost::get<0>(*i) + max)
        {
            // Count this as a propagation.
            ++ prop_b;
            ++ prop_ab[boost::get<2>(*i)];
            ++ propAB[std::make_pair(boost::get<2>(*i),uid)];

            ++ i;
            ++ o;
        }
        else
        {
            // No message could have been propagated.  Skip this incoming
            // message.
            ++ i;
        }
    }

    for (std::map<user_id, int>::citer p = m_ab.begin(); p != m_ab.end(); ++p)
    {
        stats S;
        S.m_AB = p->second;
        S.m_B_in = in.size();
        S.m_B_out = out.size();
        // S.m_A_out must be handled outside this scope.
        S.prop_B = prop_b;
        S.prop_AB = prop_ab[p->first];
        if (S.prop_B == 0)
        {
            S.energy = 0;
        }
        else
        {
            S.energy = static_cast<double>(S.prop_AB) / static_cast<double>(S.prop_B);
        }

        if (S.m_AB == 0)
        {
            S.worthy = 0;
        }
        else
        {
            S.worthy = static_cast<double>(S.prop_AB) / static_cast<double>(S.m_AB);
        }

        if (S.prop_AB >= thresh)
        {
            tmap[std::make_pair(p->first,uid)] = S;
        }
    }
}
