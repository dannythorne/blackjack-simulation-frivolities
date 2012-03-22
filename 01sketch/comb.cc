
#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iomanip>
using namespace std;

int main( int argc, char** argv)
{
  bool verbose = false;
  bool check_psum = false;

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

  const float initial_bankroll = 20;
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
  vector<int> winning_game_count( max_rounds+1, 0);
  vector<float> winning_game_prob( max_rounds+1, 0);
  vector<float> cumulative_amount_won( max_rounds+1, 0);

  int game;
  int round;

  for( num_rounds=1; num_rounds<=max_rounds; num_rounds++)
  {
    num_games[num_rounds] = num_games[num_rounds-1]*2;

    cout << "Simulating " << num_games[num_rounds]
         << " games of " << num_rounds
         << " rounds." << endl;

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
          if( verbose)
          {
            if( round==1)
            {
              cout << string( 2*(max_rounds - num_rounds), ' ');
            }
            cout << " 1";
          }

          if( !bankrupt)
          {
            win_count[round]++;
            bankroll+=wager;
            wager = base_wager;
          }

          pgame*=p;
        }
        else
        {
          // loss
          if( verbose)
          {
            if( round==1)
            {
              cout << string( 2*(max_rounds - num_rounds), ' ');
            }
            cout << " 0";
          }

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
              if( !allow_negative_bankroll && wager > bankroll)
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

      if( verbose)
      {
        cout << " -- pgame = " << setw(10) << left << pgame;
      }
      if( !bankrupt)
      {
        if( verbose)
        {
          cout << ";  bankroll  $" << setw(3) << right << bankroll;
        }
        if( bankroll > initial_bankroll)
        {
          if( verbose)
          {
            cout << " <-- winning game (up $"
                 << bankroll-initial_bankroll << ")";
          }

          if( bankroll > initial_bankroll)
          {
            winning_game_count[num_rounds]++;
            winning_game_prob[num_rounds]+=pgame;
            cumulative_amount_won[num_rounds]+=(bankroll-initial_bankroll);
          }
        }
        else if( bankroll < initial_bankroll)
        {
          //cout << " (down)";
        }
      }
      else
      {
        if( verbose)
        {
          cout << "; *bankrupt*";
        }
      }
      if( verbose)
      {
        cout << endl;
      }
      psum+=pgame;

    }

    if( verbose)
    {
      cout << "  Number of winning games: "
           << winning_game_count[num_rounds]
           << " (ratio "
           << (double)winning_game_count[num_rounds] / num_games[num_rounds]
           << ")"
           << endl;
      cout << endl;
    }

    if( check_psum)
    {
      if( psum-1.0 >= 1.0/(num_games[num_rounds]-1))
      {
        cout << "                             *** "
             << "psum = "
             << psum
             << " <-- ERROR: psum should be 1"
             << endl;
      }
      else
      {
        cout << "                             *** "
             << "psum = "
             << psum
             << endl;
      }
    }
  }


//------------------------------------------------------------------------------

  ofstream fout;
  fout.open("comb.m");

  fout << "round_count = [";
  for( round=0; round<max_rounds; round++)
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
  for( round=0; round<max_rounds; round++)
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
  for( round=0; round<max_rounds; round++)
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
  for( round=0; round<max_rounds; round++)
  {
    fout << " " << bankroll_down_count[round];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(bankroll_down_count);" << endl;
  fout << "title('bankroll down count');" << endl;
  fout << "xlabel('round');" << endl;

  fout << endl;

  fout << "winning_game_count = [";
  for( round=1; round<=max_rounds; round++)
  {
    fout << " " << winning_game_count[round];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(winning_game_count);" << endl;
  fout << "title('winning game count');" << endl;
  fout << "xlabel('length of game (num rounds)');" << endl;

  fout << endl;

  fout << "winning_game_prob = [";
  for( round=1; round<=max_rounds; round++)
  {
    fout << " " << winning_game_prob[round];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(winning_game_prob);" << endl;
  fout << "title('winning game prob');" << endl;
  fout << "xlabel('num rounds');" << endl;

  fout << endl;

  fout << "cumulative_amount_won = [";
  for( round=1; round<=max_rounds; round++)
  {
    fout << " " << cumulative_amount_won[round];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(cumulative_amount_won);" << endl;
  fout << "title('cumulative amount won');" << endl;
  fout << "xlabel('num rounds');" << endl;

  fout << endl;

  fout << "num_games = [";
  for( round=1; round<=max_rounds; round++)
  {
    fout << " " << num_games[round];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(num_games);" << endl;
  fout << "title('num games');" << endl;
  fout << "xlabel('num rounds');" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(winning_game_count./num_games);" << endl;
  fout << "title('winning game ratios');" << endl;
  fout << "xlabel('game length (num rounds)');" << endl;
  fout << "ylabel('winning\\_game\\_count./num\\_games');" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(winning_game_prob./num_games);" << endl;
  fout << "title('winning game prob ratios');" << endl;
  fout << "xlabel('game length (num rounds)');" << endl;
  fout << "ylabel('winning\\_game\\_prob./num\\_games');" << endl;

  fout << endl;

  fout << "average_amount_won = cumulative_amount_won./win_count;" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(average_amount_won);" << endl;
  fout << "title('average\\_amount\\_won');" << endl;
  fout << "xlabel('num rounds');" << endl;
  fout << "ylabel('average\\_amount\\_won');" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(average_amount_won.*winning_game_prob);" << endl;
  fout << "title('average\\_amount\\_won.*winning\\_game\\_prob');" << endl;
  fout << "xlabel('num rounds');" << endl;
  fout << "ylabel('average\\_amount\\_won.*winning\\_game\\_prob');" << endl;

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
