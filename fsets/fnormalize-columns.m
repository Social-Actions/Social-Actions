fnorm = ones(1,130);

% 86,400 for hours as a unit
% things with unit of seconds

timeU = 8640000 ;

fnorm(6) = timeU ;
fnorm(13) = timeU ;
fnorm(14) = timeU ;
fnorm(40) = timeU ;
fnorm(41) = timeU ;
fnorm(56) = timeU ;
fnorm(57) = timeU ;
fnorm(74) = timeU ;
fnorm(75) = timeU ;
fnorm(89) = timeU ;
fnorm(90) = timeU ;
fnorm(105) = timeU ;
fnorm(106) = timeU ;
fnorm(123) = timeU ;
fnorm(124) = timeU ;

% normalize days to years for number of years on twitter
fnorm(37) = 364 ;
fnorm(86) = 364 ;

% Normalize message length by 140

lengthU = 140;

fnorm(51) = lengthU ;
fnorm(52) = lengthU ;
fnorm(100) = lengthU ;
fnorm(101) = lengthU ;

f = x./(ones(size(x,1),1) * fnorm) ;

% normalize by following by column 38

f(:,1) = f(:,1)./f(:,38) ;
f(:,2) = f(:,2)./f(:,38) ;
f(:,3) = f(:,3)./f(:,38) ;
f(:,4) = f(:,4)./f(:,38) ;
f(:,5) = f(:,5)./f(:,38) ;
f(:,8) = f(:,8)./f(:,38) ;
f(:,15) = f(:,15)./f(:,38) ;
f(:,17) = f(:,17)./f(:,38) ;
f(:,19) = f(:,19)./f(:,38) ;
f(:,25) = f(:,25)./f(:,38) ;
f(:,27) = f(:,27)./f(:,38) ;
f(:,28) = f(:,28)./f(:,38) ;
f(:,39) = f(:,39)./f(:,38) ;
f(:,44) = f(:,44)./f(:,38) ;
f(:,45) = f(:,45)./f(:,38) ;
f(:,46) = f(:,46)./f(:,38) ;
f(:,47) = f(:,47)./f(:,38) ;
f(:,48) = f(:,48)./f(:,38) ;
f(:,49) = f(:,49)./f(:,38) ;
f(:,50) = f(:,50)./f(:,38) ;
f(:,58) = f(:,58)./f(:,38) ;
f(:,59) = f(:,59)./f(:,38) ;
f(:,66) = f(:,66)./f(:,38) ;
f(:,67) = f(:,67)./f(:,38) ;
f(:,68) = f(:,68)./f(:,38) ;
f(:,69) = f(:,69)./f(:,38) ;
f(:,70) = f(:,70)./f(:,38) ;
f(:,71) = f(:,71)./f(:,38) ;
f(:,72) = f(:,72)./f(:,38) ;
f(:,73) = f(:,73)./f(:,38) ;
f(:,78) = f(:,78)./f(:,38) ;
f(:,79) = f(:,79)./f(:,38) ;


% now, change column 38 to its log * 0.01
f(:,38) = log(f(:,38))*0.01 ;


% normalize by dividing with column 87
f(:,16) = f(:,16)./f(:,87) ;
f(:,18) = f(:,18)./f(:,87) ;
f(:,24) = f(:,24)./f(:,87) ;
f(:,26) = f(:,26)./f(:,87) ;
f(:,88) = f(:,88)./f(:,87) ;
f(:,91) = f(:,91)./f(:,87) ;
f(:,92) = f(:,92)./f(:,87) ;
f(:,93) = f(:,93)./f(:,87) ;
f(:,94) = f(:,94)./f(:,87) ;
f(:,95) = f(:,95)./f(:,87) ;
f(:,96) = f(:,96)./f(:,87) ;
f(:,97) = f(:,97)./f(:,87) ;
f(:,98) = f(:,98)./f(:,87) ;
f(:,99) = f(:,99)./f(:,87) ;
f(:,107) = f(:,107)./f(:,87) ;
f(:,108) = f(:,108)./f(:,87) ;
f(:,115) = f(:,115)./f(:,87) ;
f(:,116) = f(:,116)./f(:,87) ;
f(:,117) = f(:,117)./f(:,87) ;
f(:,118) = f(:,118)./f(:,87) ;
f(:,119) = f(:,119)./f(:,87) ;
f(:,120) = f(:,120)./f(:,87) ;
f(:,121) = f(:,121)./f(:,87) ;
f(:,122) = f(:,122)./f(:,87) ;
f(:,127) = f(:,127)./f(:,87) ;
f(:,128) = f(:,128)./f(:,87) ;


% now, change column 87 to its log * 0.01
f(:,87) = log(f(:,87))*0.01 ;

%percentage of folllowers that propagate the user
f(:,54) = f(:,54)./f(:,35) ;


% make 35 = 33/35 ratio, friends/followers
% make 36 = 33/36 ratio, friends/followers
f(:,35) = log(f(:,33)./(1+f(:,35))) ;

%% buggy f(:,36) = log(f(:,33)./(1+f(:,36))) ;
%% shorting out for now
f(:,36) = f(:,36)./f(:,36) ;


% normalize by 33
f(:,34) = f(:,34)./f(:,33) ;

% seems buggy, short it out for now, should divide by 33
f(:,53) = f(:,53)./f(:,53) ;
f(:,55) = f(:,55)./f(:,55) ;

% now, change columns 33 to its log * 0.01
f(:,33) = log(f(:,33))*0.01 ;

% make this percentage of followers
f(:,103) = f(:,103)./f(:,84) ;


% make 84 = 82/84 ratio, friends/followers
% make 85 = 82/85 ratio, friends/followers
f(:,84) = log(f(:,82)./(1+f(:,84))) ;


%% buggy f(:,85) = log(f(:,82)./(1+f(:,85))) ;
%% shorting out

f(:,85) = f(:,85)./f(:,85) ;

% normalize by 82
f(:,83) = f(:,83)./f(:,82) ;

% seems buggy, short it out for now, should devide by 82
f(:,102) = f(:,102)./f(:,102) ;
f(:,104) = f(:,104)./f(:,104) ;

% now, change column 82 to its log * 0.01
f(:,82) = log(f(:,82))*0.01 ;

%% Seems buggy, shorting it out for now.
f(:,41) = f(:,41)./f(:,41) ;

