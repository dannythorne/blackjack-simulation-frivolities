
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

#include "blackjack.h"
using namespace dthorne0_blackjack;

int main()
{
  cout << __FILE__ << " " << __LINE__ << " -- "
       << "main(), Starting." << endl;

  srand( time(NULL));

  Rules rules(/*numDecks*/4);

  Table x(rules);
  if(  x.verbose()) { x.toggle_verbosity();}
  if(  x.displayHands()) { x.toggle_displayHands();}
  if(  x.displayShoe()) { x.toggle_displayShoe();}
  cout << x << endl;

  int numHandsAtATime = 100000000;

  int i;
  for( i=0; i<10; i++)
  {
    x.play(/*numHands*/numHandsAtATime);
    cout << endl;
    cout << x << endl;
  }

  cout << __FILE__ << " " << __LINE__ << " -- "
       << "main(), Terminating normally." << endl;
  return 0;
}
