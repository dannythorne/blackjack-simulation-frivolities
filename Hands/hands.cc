
#include <iostream>
using namespace std;

const int N_CARD_TYPES = 10; // Number of card types: |{ 10, A, 2-9}| = 10

int main()
{
  cout << "Hello!" << endl;

  int num_decks = 6;
  int shoe[N_CARD_TYPES];

  int i;

  for( i=0; i<N_CARD_TYPES; i++)
  {
    shoe[i] = num_decks*4;
  }
  shoe[0]*=4; // Four times as many tens.

  cout << "In a shoe with " << num_decks << " decks:" << endl;
  for( i=0; i<N_CARD_TYPES; i++)
  {
    cout << "  Card type " << i << " occurs " << shoe[i] << " times." << endl;
  }

  cout << "Good bye!" << endl;
  return 0;
}
