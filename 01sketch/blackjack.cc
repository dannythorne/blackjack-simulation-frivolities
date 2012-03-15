
#include <iostream>
#include <cstdlib>
using namespace std;

void extract_args( int argc, char** argv, int& numHands);

int main( int argc, char** argv)
{
  cout << __FILE__ << " " << __LINE__ << " -- " << "Hello." << endl;

  int numHands = 9;

  extract_args( argc, argv, numHands);

  int hand;

  for( hand=1; hand<=numHands; hand++)
  {
    cout << "hand " << hand << endl;
  }

  cout << __FILE__ << " " << __LINE__ << " -- " << "Good Bye." << endl;
  return 0;
}

void extract_args( int argc, char** argv, int& numHands)
{
  int n;
  switch( argc)
  {
    case 1: // ./a.exe
    {
      break;
    }

    case 2: // ./a.exe numHands
    {
      n = atoi(argv[1]);
      if( n) { numHands = n;}

      break;
    }

    case 3: // ./a.exe numHands asdf
    {
      break;
    }

    default:
      cout << __FILE__ << " " << __LINE__ << " -- "
           << "WARNING -- Unhandled case: argc==" << argc << endl;
  }
}
