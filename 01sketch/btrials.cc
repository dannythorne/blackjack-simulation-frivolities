
#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <ctime>
using namespace std;

int main( int argc, char** argv)
{
  time_t seed = 0;

  if( /*random seed*/ true)
  {
    seed = time(NULL);
    ofstream seedout;
    seedout.open("btrials_seeds.txt",ios::app);
    seedout << seed << endl;
    seedout.close();
  }
  else
  {
    seed = 1;

  }

  srand( seed);

  float p = 0.469214;
  float pbing = 0.105522; // simulate occasional blackjack payout
  int num_trials = 1e8;

  int max_rounds = 1e4; //4e3;

  float initial_bankroll = 200;
  float base_wager = 1;

  bool do_martingale = false;

  float bankroll = initial_bankroll;
  float wager = base_wager;

  vector<int> win_count( max_rounds+1, 0);
  vector<int> bankroll_up_count( max_rounds+1, 0);
  vector<int> bankroll_down_count( max_rounds+1, 0);
  vector<int> round_count( max_rounds+1, 0);
  vector<int> bankroll_cumulative( max_rounds+1, 0);
  vector<int> bankroll_first_game( max_rounds+1, 0);

  int trial;
  int round = 1;
  int num_rounds = 0;
  bool first_game = true;
  int num_rounds_first_game = 0;
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

      bankroll_cumulative[round]+=bankroll;

      if( first_game)
      {
        bankroll_first_game[round]=bankroll;
      }

      if( bankroll > base_wager)
      {
        wager = base_wager;
      }
      else
      {
        wager = bankroll;
      }
    }
    else
    {
      // loss
      bankroll-=wager;
      bankroll_cumulative[round]+=bankroll;

      if( first_game)
      {
        bankroll_first_game[round]=bankroll;
      }

      if( bankroll <= 0)
      {
        // bankrupt
        if( first_game)
        {
          num_rounds_first_game = round;
          first_game = false;
        }
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
      if( first_game)
      {
        num_rounds_first_game = round;
        first_game = false;
      }
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
  fout << "title('btrials: round count');" << endl;
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
  fout << "title('btrials: win count');" << endl;
  fout << "xlabel('round');" << endl;

  fout << endl;

  fout << "bankroll_cumulative = [";
  for( round=0; round<num_rounds; round++)
  {
    fout << " " << bankroll_cumulative[round];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(bankroll_cumulative);" << endl;
  fout << "title('btrials: bankroll cumulative');" << endl;
  fout << "xlabel('round');" << endl;

  fout << endl;

  fout << "bankroll_first_game = [";
  for( round=1; round<num_rounds_first_game; round++)
  {
    fout << " " << bankroll_first_game[round];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(bankroll_first_game);" << endl;
  fout << "title('btrials: bankroll first game');" << endl;
  fout << "xlabel('round');" << endl;
  fout << "print('00btrials_bankroll_first_game.png','-dpng','-r128');"
       << endl;

  fout << endl;

  fout << "bankroll_up_count = [";
  for( round=0; round<num_rounds; round++)
  {
    fout << " " << bankroll_up_count[round];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(bankroll_up_count);" << endl;
  fout << "title('btrials: bankroll up count');" << endl;
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
  fout << "title('btrials: bankroll down count');" << endl;
  fout << "xlabel('round');" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(bankroll_down_count./round_count);" << endl;
  fout << "title('btrials: bankroll down ratios');" << endl;
  fout << "xlabel('round');" << endl;
  fout << "ylabel('bankroll\\_down\\_count./round\\_count');" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(bankroll_up_count./round_count);" << endl;
  fout << "title('btrials: bankroll up ratios');" << endl;
  fout << "xlabel('round');" << endl;
  fout << "ylabel('bankroll\\_up\\_count./round\\_count');" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(bankroll_up_count./bankroll_down_count);" << endl;
  fout << "title('btrials: bankroll up/down ratios');" << endl;
  fout << "xlabel('round');" << endl;
  fout << "ylabel('bankroll\\_up\\_count./bankroll\\_down\\_count');" << endl;


  fout << "win_ratio = win_count./round_count;" << endl;
  fout << "up_ratio = bankroll_up_count./round_count;" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(up_ratio.*win_ratio);" << endl;
  fout << "title('btrials: up ratio times win ratio');" << endl;
  fout << "xlabel('round');" << endl;
  fout << "ylabel('up\\_ratio.*win\\_ratio');" << endl;


  fout << endl;

  fout.close();

  return 0;
}
