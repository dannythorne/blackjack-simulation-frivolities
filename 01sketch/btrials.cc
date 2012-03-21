
#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
using namespace std;

int main()
{
  float p = 0.469214;
  float pbing = 0.105522; // simulate occasional blackjack payout
  int num_trials = 1e9;
  int max_rounds = 1e3;

  float initial_bankroll = 40;
  float base_wager = 1;

  bool do_martingale = true;

  float bankroll = initial_bankroll;
  float wager = base_wager;

  vector<int> win_count( max_rounds+1, 0);
  vector<int> bankroll_up_count( max_rounds+1, 0);
  vector<int> bankroll_down_count( max_rounds+1, 0);
  vector<int> round_count( max_rounds+1, 0);

  int trial;
  int round = 1;
  int num_rounds = 0;
  for( trial=0; trial<num_trials; trial++)
  {
    round_count[round]++;

    if( (double)rand()/RAND_MAX < p)
    {
      // win
      win_count[round]++;
      bankroll+=wager;
      if( (double)rand()/RAND_MAX < pbing)
      {
        bankroll+=0.5*wager;
      }
      wager = base_wager;
    }
    else
    {
      // loss
      bankroll-=wager;

      if( bankroll <= 0)
      {
        // bankrupt
        round = 1;
        wager = base_wager;
        bankroll = initial_bankroll;
      }
      else
      {
        if( do_martingale)
        {
          // double wager (Martingale strategy)
          wager*=2;
        }
        if( wager > bankroll)
        {
          wager = bankroll;
        }
      }
    }

    if( bankroll > initial_bankroll)
    {
      bankroll_up_count[round]++;
    }
    else if( bankroll < initial_bankroll)
    {
      bankroll_down_count[round]++;
    }

    if( round > num_rounds)
    {
      num_rounds = round;
    }
    round++;

    if( round > max_rounds)
    {
      round = 1;
      wager = base_wager;
      bankroll = initial_bankroll;
    }
  }

//------------------------------------------------------------------------------

  ofstream fout;
  fout.open("btrials.m");

  fout << "round_count = [";
  for( round=0; round<num_rounds; round++)
  {
    fout << " " << round_count[round];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(round_count);" << endl;
  fout << "title('round count');" << endl;
  fout << "xlabel('round');" << endl;

  fout << endl;

  fout << "win_count = [";
  for( round=0; round<num_rounds; round++)
  {
    fout << " " << win_count[round];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(win_count);" << endl;
  fout << "title('win count');" << endl;
  fout << "xlabel('round');" << endl;

  fout << endl;

  fout << "bankroll_up_count = [";
  for( round=0; round<num_rounds; round++)
  {
    fout << " " << bankroll_up_count[round];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(bankroll_up_count);" << endl;
  fout << "title('bankroll up count');" << endl;
  fout << "xlabel('round');" << endl;

  fout << endl;

  fout << "bankroll_down_count = [";
  for( round=0; round<num_rounds; round++)
  {
    fout << " " << bankroll_down_count[round];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(bankroll_down_count);" << endl;
  fout << "title('bankroll down count');" << endl;
  fout << "xlabel('round');" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(bankroll_down_count./round_count);" << endl;
  fout << "title('bankroll down ratios');" << endl;
  fout << "xlabel('round');" << endl;
  fout << "ylabel('bankroll\\_down\\_count./round\\_count');" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(bankroll_up_count./round_count);" << endl;
  fout << "title('bankroll up ratios');" << endl;
  fout << "xlabel('round');" << endl;
  fout << "ylabel('bankroll\\_up\\_count./round\\_count');" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(bankroll_up_count./bankroll_down_count);" << endl;
  fout << "title('bankroll up/down ratios');" << endl;
  fout << "xlabel('round');" << endl;
  fout << "ylabel('bankroll\\_up\\_count./bankroll\\_down\\_count');" << endl;


  fout << endl;

  fout.close();

  return 0;
}
