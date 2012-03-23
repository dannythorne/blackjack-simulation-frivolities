
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
using namespace std;

struct BTrial
{
  float p;
  float pbing;
  int num_trials;
  int max_trials;
  int base_wager;
  int max_wager;
  int num_wins;
  int winnings;
};

void init( BTrial& btrial);
void play( BTrial& btrial);

int main()
{
  srand(time(0));

  BTrial btrial;

  init( btrial);

  float ave_wins;
  float win_ratio;
  float ave_winnings;
  float ave_positive;

  int n = 1e2;

  int i;

  cout << "%% num_trials ave_wins ave_winnings ave_positive" << endl;
  cout << "x = [" << endl;

  for( btrial.num_trials=1;
       btrial.num_trials<=btrial.max_trials;
       btrial.num_trials++)
  {
    ave_wins = 0;
    ave_winnings = 0;
    ave_positive = 0;

    for( i=0; i<n; i++)
    {
      play( btrial);

      ave_wins += btrial.num_wins;
      ave_winnings += btrial.winnings;
      if( btrial.winnings > 0)
      {
        ave_positive+=1;
      }
    }

    ave_wins /= n;
    ave_winnings /= n;
    ave_positive /= n;
    win_ratio = (double)ave_wins / btrial.num_trials;

    cout << setw(4) << btrial.num_trials
         << " "
         << fixed << setprecision(8) << win_ratio
         << " "
         << setw(5) << ave_winnings
         << " "
         << setw(5) << ave_winnings * win_ratio
         << " "
         << ave_positive
         << ";"
         << endl;
  }

  cout << "];" << endl;
  cout << "plot(x(:,end)); "
       << "axis([ 0 " << btrial.num_trials << " 0 1]);" << endl;

  return 0;
}

void init( BTrial& btrial)
{
  btrial.p = 0.469;
  btrial.pbing = 0.0494;
  btrial.max_trials = 1e4;
  btrial.base_wager = 1;
  btrial.max_wager = 128;
}

void play( BTrial& btrial)
{
  btrial.num_wins = 0;
  int wager = btrial.base_wager;
  btrial.winnings = 0;

  int i;
  for( i=0; i<btrial.num_trials; i++)
  {
    if( (double)rand()/(double)RAND_MAX < btrial.p)
    {
      btrial.num_wins++;
      btrial.winnings += wager;
      if( (double)rand()/RAND_MAX < btrial.pbing)
      {
        btrial.winnings+=(wager/2.0);
      }
      wager = btrial.base_wager;
    }
    else
    {
      btrial.winnings -= wager;
      wager*=2.0;
      if( wager > btrial.max_wager)
      {
        wager = btrial.base_wager;
        //return;
      }
    }
  }
}
