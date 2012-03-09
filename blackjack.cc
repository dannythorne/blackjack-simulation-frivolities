
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
using namespace std;

int main( int argc, char** argv)
{
  cout << "main() -- Hello." << endl;

  int hand;
  int numHands;
  int maxNumHands = 10;
  int numGames = 10;
  double probWin = 0.424249 / ( 0.424249 + 0.490964);
  double r;
  int n;
  int numHandsWon;
  int game;
  double winRatio;
  double sumWinRatio;
  int bankRoll;
  int initBankRoll = 20;
  int wager;
  int maxWager = 32;
  int numGamesWon;
  double ratioGamesWon;
  int percentGamesWon;
  bool do_martingale = true;

  if( argc > 1)
  {
    switch( argc)
    {
      case 2:
      {
        if( atoi( argv[1])>0)
        {
          maxNumHands = atoi( argv[1]);
        }
        break;
      }
      case 3:
      {
        if( atoi( argv[1])>0)
        {
          maxNumHands = atoi( argv[1]);
        }
        if( atoi( argv[2])>0)
        {
          numGames = atoi( argv[2]);
        }
        break;
      }
      default:
      {
        cout
          << __FILE__
          << " "
          << __LINE__
          << " -- "
          << "Unhandled case: argc == "
          << argc
          << endl;
        break;
      }
    }
  }

  for( numHands=1; numHands<=maxNumHands; numHands++)
  {
    sumWinRatio = 0.0;
    numGamesWon = 0;

    for( game=1; game<=numGames; game++)
    {
      numHandsWon = 0;
      bankRoll = initBankRoll;
      wager = 1;

      for( hand=1; hand<=numHands; hand++)
      {
        r = rand() / (double)RAND_MAX;

        if( r < probWin)
        {
          numHandsWon++;
          bankRoll += wager;
          if( do_martingale)
          {
            wager = 1;
          }
        }
        else
        {
          bankRoll -= wager;
          if( do_martingale)
          {
            wager *= 2;
            if( wager>maxWager) { wager = 1;}
          }
        }
      }

      winRatio = numHandsWon / (double)numHands;
      sumWinRatio += winRatio;
      if( bankRoll > initBankRoll)
      {
        numGamesWon++;
      }
    }

    ratioGamesWon = numGamesWon / (double)numGames;
    percentGamesWon  = (int)floor( 100 * ratioGamesWon);

    if( false)
    {
      cout
        << numHands
        << " aveWinRatio = "
        << sumWinRatio / (double)numGames
        << ", numGamesWon = "
        << numGamesWon
        << " ( "
        << percentGamesWon
        << " % )"
        << endl;
    }
    else
    {
      cout
        << string(percentGamesWon-1,'-')
        << "*"
        << endl;
    }
  }

  cout << "main() -- Bye." << endl;
  return 0;
}
