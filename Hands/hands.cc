
#include <iostream>
using namespace std;

const int N_CARD_TYPES = 10; // Number of card types: |{ 10, A, 2-9}| = 10

const int g_limit_num_cards_per_hand = 0;

void deal( int* hand, int* shoe, int num_cards);
int value_of( int* hand, int num_cards);
void show( int* hand, int num_cards);

int main()
{
  cout << "Hello!" << endl;

  int num_decks = 2;
  int shoe[N_CARD_TYPES];

  int hand[21 + 1];

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

  cout << "Good bye!" << endl;
  return 0;
}

void deal( int* hand, int* shoe, int num_cards)
{
  if( g_limit_num_cards_per_hand)
  {
    if( num_cards>=g_limit_num_cards_per_hand)
    {
      hand[21] = value_of( hand, num_cards);
      show( hand, num_cards);
      return;
    }
  }

  int i;
  for( i=0; i<N_CARD_TYPES; i++)
  {
    if( shoe[i] > 0)
    {
      hand[num_cards] = i;
      shoe[i]--;

      hand[21] = value_of( hand, num_cards+1);
      if( hand[21] < 17) // (just stand on soft 17 for now)
      {
        deal( hand, shoe, num_cards+1);
      }
      else
      {
        show( hand, num_cards+1);
      }
      shoe[i]++;
    }
  }
}

int value_of( int* hand, int num_cards)
{
  int val = 0;
  int num_aces = 0;
  int i;

  for( i=0; i<num_cards; i++)
  {
    if( hand[i] == 0)
    {
      val += 10;
    }
    else if( hand[i] != 1)
    {
      val += hand[i];
    }
    else
    {
      val += 11;
      num_aces++;
    }
  }

  while( val > 21 && num_aces>0)
  {
    val -= 10;
    num_aces--;
  }

  return val;
}

void show( int* hand, int num_cards)
{
  int i;
  cout << " " << hand[0] << " [";
  for( i=1; i<num_cards; i++)
  {
    cout << " " << hand[i];
  }
  cout << "] ";
  cout << hand[21];
  if( hand[21] > 21)
  {
    cout << " (Bust)";
  }
  cout << endl;
}
