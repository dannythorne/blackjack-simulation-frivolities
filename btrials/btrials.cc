
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
using namespace std;

void play( const float p
         , const float pbing
         , const int num_trials
         , const int base_wager
         , const int max_wager
         , int &num_wins
         , int &winnings );

int main()
{
  srand(time(0));

  int num_trials;
  int num_wins;
  int winnings;

  float ave_wins;
  float ave_winnings;
  float ave_positive;

  float p = 0.49;
  float pbing = 0.04;

  int max_trials = 3e3;
  int base_wager = 1;
  int max_wager = 128;
  int n = 1e3;

  int i;

  cout << "%% num_trials ave_wins ave_winnings ave_positive" << endl;
  cout << "x = [" << endl;

  for( num_trials=1; num_trials<=max_trials; num_trials++)
  {
    ave_wins = 0;
    ave_winnings = 0;
    ave_positive = 0;

    for( i=0; i<n; i++)
    {
      play( p, pbing, num_trials, base_wager, max_wager, num_wins, winnings);

      ave_wins += num_wins;
      ave_winnings += winnings;
      if( winnings > 0)
      {
        ave_positive+=1;
      }
    }

    ave_wins /= n;
    ave_winnings /= n;
    ave_positive /= n;

    cout << setw(4) << num_trials
         << " "
         << fixed << setprecision(8) << (double)ave_wins / num_trials
         << " "
         << setw(5) << ave_winnings
         << " "
         << ave_positive
         << ";"
         << endl;
  }

  cout << "];" << endl;
  cout << "figure; plot(x(:,end)); "
       << "axis([ 0 " << num_trials << " 0 1]);" << endl;

  return 0;
}

void play( const float p
         , const float pbing
         , const int num_trials
         , const int base_wager
         , const int max_wager
         , int &num_wins
         , int &winnings
         )
{
  num_wins = 0;
  int wager = base_wager;
  winnings = 0;

  int i;
  for( i=0; i<num_trials; i++)
  {
    if( (double)rand()/(double)RAND_MAX < p)
    {
      num_wins++;
      winnings += wager;
      if( (double)rand()/RAND_MAX < pbing) { winnings+=(wager/2);}
      wager = base_wager;
    }
    else
    {
      winnings -= wager;
      wager*=2;
      if( wager > max_wager) { wager = base_wager;}
    }
  }
}
