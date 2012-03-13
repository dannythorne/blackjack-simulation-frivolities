
#include <iostream>
using namespace std;

const int N_CARD_TYPES = 10; // Number of card types: |{ 10, A, 2-9}| = 10

void deal( int* hand, int* shoe, int num_cards);
int value_of( int* hand, int num_cards);
void show( int* hand, int num_cards);

int main()
{
  cout << "Hello!" << endl;

  int num_decks = 6;
  int shoe[N_CARD_TYPES];

  int hand[21];

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

  deal( hand, shoe, 0);

  cout << "Test deal card: " << hand[0] << endl;

  cout << "Good bye!" << endl;
  return 0;
}

void deal( int* hand, int* shoe, int num_cards)
{
  if( num_cards>2) // temporary until value_of is implemented
  {
    show( hand, num_cards);
    return;
  }

  int i;
  for( i=0; i<10; i++)
  {
    hand[num_cards] = i;

    if( value_of( hand, num_cards) < 17) // (just stand on soft 17 for now)
    {
      deal( hand, shoe, num_cards+1);
    }
    else
    {
      show( hand, num_cards);
    }
  }
}

int value_of( int* hand, int num_cards)
{
  return 1;
}

void show( int* hand, int num_cards)
{
  int i;
  cout << " " << hand[0] << " [";
  for( i=1; i<num_cards; i++)
  {
    cout << " " << hand[i];
  }
  cout << "]" << endl;
}
