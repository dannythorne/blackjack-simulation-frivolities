
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

#include "blackjack.h"
using namespace dthorne0_blackjack;

int main()
{
  cout << __FILE__ << " " << __LINE__ << " -- "
       << "main(), Starting." << endl;

  srand( time(NULL));

  Rules rules( /*numDecks*/ 8
             , /*maxPenetrationRatio*/ 0.7
             , /*blackjackPayoutFactor*/ 1.5 );

  Table x(rules);
  if(  x.verbose()) { x.toggle_verbosity();}
  if(  x.displayHands()) { x.toggle_displayHands();}
  if(  x.displayShoe()) { x.toggle_displayShoe();}

  ofstream fout;
  fout.open("dump.m");

  fout << x << endl;

  int numHands = 1e8;

  cout << "Playing " << numHands << " hands." << endl;
  x.play(numHands);

  cout << "Writing results to \"dump.m\"." << endl;
  fout << x << endl;

  fout.close();

  cout << __FILE__ << " " << __LINE__ << " -- "
       << "main(), Terminating normally." << endl;
  return 0;
}
