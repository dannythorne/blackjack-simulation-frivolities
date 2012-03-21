
#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iomanip>
using namespace std;

int main( int argc, char** argv)
{
  int max_rounds = 0;

  if( argc > 1)
  {
    max_rounds = atoi( argv[1]);
  }

  if( !max_rounds)
  {
    max_rounds = 3;
  }

  const float p = 0.469214;
  const float q = 1-p;
  float pgame;
  float psum;
  float tol = 1.2e-6;

  int num_rounds;
  vector<int> num_games(max_rounds+1,1);

  const float initial_bankroll = 3;
  const float base_wager = 1;

  const bool do_martingale = true;

  float bankroll;
  float wager;

  const bool allow_negative_bankroll = false;
  bool bankrupt;

  vector<int> win_count( max_rounds+1, 0);
  vector<int> bankroll_up_count( max_rounds+1, 0);
  vector<int> bankroll_down_count( max_rounds+1, 0);
  vector<int> round_count( max_rounds+1, 0);

  int game;
  int round;

  for( num_rounds=1; num_rounds<=max_rounds; num_rounds++)
  {
    num_games[num_rounds] = num_games[num_rounds-1]*2;

    psum = 0;
    for( game=0; game<num_games[num_rounds]; game++)
    {
      bankroll = initial_bankroll;
      wager = base_wager;
      pgame = 1;
      bankrupt = false;

      for( round=1; round<=num_rounds; round++)
      {
        round_count[round]++;
        //
        // num_rounds       3
        // num_games: 1 2 4 8
        // round      0 1 2 3
        //             -------
        //              0 0 0 <-- game 0
        //              0 0 1 <-- game 1
        //              0 1 0 <-- game 2
        //              0 1 1 <-- game 3
        //              1 0 0 <-- game 4
        //              1 0 1 <-- game 5
        //              1 1 0 <-- game 6
        //              1 1 1 <-- game 7
        //
        if( game & num_games[num_rounds-round])
        {
          // win
          cout << " 1";

          if( !bankrupt)
          {
            win_count[round]++;
            bankroll+=wager;
          }

          pgame*=p;
        }
        else
        {
          // loss
          cout << " 0";

          if( !bankrupt)
          {
            bankroll-=wager;

            if( !allow_negative_bankroll && bankroll <= 0)
            {
              bankrupt = true;
            }
            else
            {
              if( do_martingale)
              {
                wager*=2;
              }
              if( wager > bankroll)
              {
                wager = bankroll;
              }
            }
          }
          pgame*=q;
        }

        if( !bankrupt)
        {
          if( bankroll > initial_bankroll)
          {
            bankroll_up_count[round]++;
          }
          else if( bankroll < initial_bankroll)
          {
            bankroll_down_count[round]++;
          }
        }
      }

      cout << " -- pgame = " << pgame;
      if( !bankrupt)
      {
        cout << ";  bankroll  $" << bankroll;
        if( bankroll > initial_bankroll)
        {
          cout << " WINNER";
        }
        else if( bankroll < initial_bankroll)
        {
          //cout << " (down)";
        }
      }
      else
      {
        cout << "; *bankrupt*";
      }
      cout << endl;
      psum+=pgame;

    }

    cout << endl;
    if( psum-1.0 >= tol)
    {
      cout << "ERROR: "
           << "psum = "
           << setprecision(20) << psum
           << " <-- should be 1"
           << endl;
    }
  }


//------------------------------------------------------------------------------

  ofstream fout;
  fout.open("comb.m");

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


  fout << "win_ratio = win_count./round_count;" << endl;
  fout << "up_ratio = bankroll_up_count./round_count;" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(up_ratio.*win_ratio);" << endl;
  fout << "title('up ratio times win ratio');" << endl;
  fout << "xlabel('round');" << endl;
  fout << "ylabel('up\\_ratio.*win\\_ratio');" << endl;


  fout << endl;

  fout.close();



  return 0;
}
