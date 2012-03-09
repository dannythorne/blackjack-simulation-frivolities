
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <ctime>
using namespace std;

void displayHand( const char* label, int* hand, int n);
int computeHandValue( int* hand, int n, int& numAces);
bool playerShouldHit( int* hand, int n, int dealerCard);
bool playerShouldDouble( int* hand, int n, int dealerCard);
bool playerShouldSplit( int* hand, int n, int dealerCard);
bool dealerShouldHit( int* hand, int n);

int main()
{
  cout << __FILE__ << " " << __LINE__ << " -- main(), Hello!" << endl;
  cout << endl;

  srand( time(NULL));

  bool showHands = false;

  int numCardsPerSuit = 13;
  int numSuits = 4;
  int numCardsPerDeck = numSuits*numCardsPerSuit;
  int numDecks = 6;
  int numCards = numDecks*numCardsPerDeck;

  cout << "numDecks: " << numDecks << endl;
  cout << "numCards: " << numCards << endl;

  int *decks;
  decks = new int[numCards];
  int *count;
  count = new int[numCards+1];
  count[0] = 0;

  int i, j, temp;

  // Initialize decks and count.
  for( i=0; i<numCards; i++)
  {
    decks[i] = i%numCardsPerSuit;
  }

  // Shuffle decks.
  for( i=0; i<numCards*numCards; i++)
  {
    j = (int)floor(numCards*(rand()/(double)RAND_MAX));
    temp = decks[j];
    decks[j] = decks[0];
    decks[0] = temp;
  }

  for( i=0; i<numCards; i++)
  {
    count[i+1] = count[i]
               + ( decks[i]>0 && decks[i]<7)
               - ( decks[i]>9);
  }

  int maxHands = 10000;
  int hand = 0;
  int card = 0;
  int reshuffleThresh = (int)floor(0.70*numCards);
  int handDealer[21];
  int numCardsDealer = 0;
  int handPlayer[21];
  int numCardsPlayer = 0;
  int handPlayerValue = 0;
  int handDealerValue = 0;
  int numAcesPlayer = 0;
  int numAcesDealer = 0;
  int losing = 0;
  int maxLosingStreak = 0;
  double wager = 1;
  int maxWager = 1;
  double wagerIncFactor=2.0;
  //unsigned int maxAllowableWager = -1;
  unsigned int maxAllowableWager = 128;
  //unsigned int maxAllowableWager = pow(wagerIncFactor,7);
  double* winnings;
  winnings = new double[maxHands+1];
  winnings[0] = 0.0;
  double minWinnings = 0;
  double maxWinnings = 0;
  int numPlayerBlackJacks = 0;
  int numDealerBlackJacks = 0;
  bool dealerBlackJack = false;
  bool playerBlackJack = false;
  bool betByCount = true;
  bool allowDouble = true;
  bool allowSplit = false;
  bool doubleDown = false;

  while( hand < maxHands)
  {
    if( wager > maxWager) { maxWager = wager;}

    if( losing > maxLosingStreak) { maxLosingStreak = losing;}

    if( showHands) { cout << string(80,'=') << endl;}

    numCardsDealer = 0;
    numCardsPlayer = 0;

    numAcesPlayer = 0;
    numAcesDealer = 0;

    dealerBlackJack = false;
    playerBlackJack = false;

    doubleDown = false;

    // First two cards to player and dealer.
    handPlayer[numCardsPlayer++] = decks[card++];
    handDealer[numCardsDealer++] = decks[card++];
    handPlayer[numCardsPlayer++] = decks[card++];
    handDealer[numCardsDealer++] = decks[card++];

    handPlayerValue =
      computeHandValue( handPlayer, numCardsPlayer, numAcesPlayer);
    handDealerValue =
      computeHandValue( handDealer, numCardsDealer, numAcesDealer);

    // Check for blackjack.
    if( handPlayerValue == 21)
    {
      playerBlackJack = true;
      numPlayerBlackJacks++;
    }
    else if( allowDouble
          && playerShouldDouble( handPlayer, numCardsPlayer, handDealer[0]))
    {
      doubleDown = true;
      wager*=2;
      handPlayer[numCardsPlayer++] = decks[card++];
      handPlayerValue =
        computeHandValue( handPlayer, numCardsPlayer, numAcesPlayer);
    }
    else if( allowSplit
          && playerShouldSplit( handPlayer, numCardsPlayer, handDealer[0]))
    {
      // TODO: Split
    }
    else
    {
      // Deal to player.
      while( playerShouldHit( handPlayer, numCardsPlayer, handDealer[0]))
      {
        handPlayer[numCardsPlayer++] = decks[card++];
        handPlayerValue =
          computeHandValue( handPlayer, numCardsPlayer, numAcesPlayer);
      }
    }

    if( handDealerValue == 21)
    {
      dealerBlackJack = true;
      numDealerBlackJacks++;
    }
    else
    {
      // Deal to dealer.
      while( dealerShouldHit(handDealer,numCardsDealer))
      {
        handDealer[numCardsDealer++] = decks[card++];
        handDealerValue =
          computeHandValue( handDealer, numCardsDealer, numAcesDealer);
      }
    }

    // Show hands.
    if( showHands)
    {
      displayHand( "Dealer", handDealer, numCardsDealer);
      if( handDealerValue <= 21 && handDealerValue > handPlayerValue)
      {
        cout << " Winner!";
      }
      cout << endl;
      displayHand( "Player", handPlayer, numCardsPlayer);
      if( handPlayerValue <=21
       && ( handPlayerValue > handDealerValue || handDealerValue > 21))
      {
        cout << " Winner!";
        cout << " ($" << wager << ")";
      }
      if( handPlayerValue>21
       || ( handDealerValue <= 21 && handDealerValue > handPlayerValue))
      {
        cout << " (Lost " << losing+1 << " a row.)";
      }
      cout << endl;
    }

    // Update winnings, etc.
    if( handPlayerValue <= 21)
    {
      if( handPlayerValue > handDealerValue || handDealerValue > 21)
      {
        winnings[hand+1] = winnings[hand] + wager;
        if( playerBlackJack)
        {
          winnings[hand+1] = winnings[hand+1] + wager/2.0;
        }
        losing = 0;
      }
      else if( handDealerValue <=21 && handDealerValue > handPlayerValue)
      {
        winnings[hand+1] = winnings[hand] - wager;
        wager*=wagerIncFactor;
        losing++;
      }
      else if( handDealerValue <=21)
      {
        if( showHands) { cout << "Push." << endl;}
        winnings[hand+1] = winnings[hand];
      }
      else
      {
        cout << __FILE__ << " " << __LINE__ << "Yikes!" << endl;
        exit(1);
      }
    }
    else
    {
      winnings[hand+1] = winnings[hand] - wager;
      wager*=wagerIncFactor;
      losing++;
    }

    hand++;

    if( losing == 0) { wager = 1;}

    cout << hand << " $" << winnings[hand] << endl;

    // Reshuffle when necessary.
    if( card>reshuffleThresh)
    {
      cout << " -- Reshuffling after card " << card << ". ("
           << card << "/"
           << numCards << "="
           << (double)card/(double)numCards
           << ")" << endl;
      for( i=0; i<numCards*numCards; i++)
      {
        j = (int)floor(numCards*(rand()/(double)RAND_MAX));
        temp = decks[j];
        decks[j] = decks[0];
        decks[0] = temp;
      }

      for( i=0; i<numCards; i++)
      {
        count[i+1] = count[i]
                   + ( decks[i]>0 && decks[i]<7)
                   - ( decks[i]>9);
      }

      card = 0;
      //wager = 1;
    }
    else
    {
      if( betByCount)
      {
        wager = count[card];
        if( wager < 10) { wager = 1;}
      }
    }

    if( winnings[hand] > maxWinnings) { maxWinnings = winnings[hand];}
    if( winnings[hand] < minWinnings) { minWinnings = winnings[hand];}

    if( wager > maxAllowableWager)
    {
      wager = 1;
      losing = 0;
      cout << "Cutting losses at hand " << hand << ". "
           << "(Wager exceeded $" << maxAllowableWager << ".)" << endl;
    }

  } // while( hand < maxHands)

  cout << endl;
  cout << endl;
  cout << "minWinnings      : " << minWinnings       << endl;
  cout << "maxWinnings      : " << maxWinnings       << endl;
  cout << "maxWager         : " << maxWager          << endl;
  cout << "maxLosingStreak  : " << maxLosingStreak   << endl;
  cout << "maxAllowableWager: " << maxAllowableWager << endl;
  cout << "wagerIncFactor   : " << wagerIncFactor    << endl;

  ofstream fout;
  fout.open("winnings.txt");
  for( i=0; i<=maxHands; i++)
  {
    fout << winnings[i] << endl;
  }
  fout.close();

  delete [] winnings;
  delete [] decks;

  cout << endl;
  cout << __FILE__ << " " << __LINE__ << " -- main(), "
       << "Terminating normally!"
       << endl;
  return 0;
}

void displayHand( const char* label, int* hand, int n)
{
  int i;
  cout << label << ": ";
  int numAces;
  int handValue = computeHandValue( hand, n, numAces);
  cout << handValue << " [";

  for( i=0; i<n; i++)
  {
    if( i==2) { cout << ",";}
    switch( hand[i])
    {
      case  0: cout << " A"; break;
      case 10: cout << " 0"; break;
      case 11: cout << " J"; break;
      case 12: cout << " Q"; break;
      case 13: cout << " K"; break;
      default: cout << " " << hand[i];
    }
  }
  cout << "]";
  if( handValue > 21)
  {
    cout << " BUST";
  }
  if( handValue == 21)
  {
    cout << " BLACKJACK";
  }
  if( numAces>0)
  {
    if( numAces>1)
    {
      cout << " (" << numAces << " Aces at 11pts)";
    }
    else
    {
      cout << " (" << numAces << " Ace at 11pts)";
    }
  }
}

int computeHandValue( int* hand, int n, int& numAces)
{
  int i = 0;
  numAces = 0;
  int val = 0;
  while( i<n)
  {
    if( hand[i]==0) { numAces++; val+=11;}
    else
    {
      if( hand[i]>9) { val+=10;}
      else { val+=hand[i];}
    }
    i++;
  }
  if( val > 21)
  {
    while( numAces > 0 && val > 21)
    {
      val-=10;
      numAces--;
    }
  }
  return val;
}


bool dealerShouldHit( int* hand, int n)
{
  int numAces;
  return computeHandValue( hand, n, numAces) < 17;
}

bool playerShouldHit(
       int* hand, int n, int dealerCard)
{
  int numAces;
  int handValue = computeHandValue( hand, n, numAces);

  if( handValue==21) { return false;}

  if(
      !(handValue > 17)
    && (
         handValue < 12
      ||
         ( handValue > 12 && ( dealerCard > 6 || !dealerCard))
      ||
         ( handValue==12 && ( dealerCard > 6 || dealerCard < 4 ))
       )
    )
  {
    return true;
  }
//else if( numAces > 0 && handValue-10 < 12)
//{
//  return true;
//}
  else
  {
    return false;
  }
}

bool playerShouldDouble( int* hand, int n, int dealerCard)
{
  if( ( dealerCard > 2 || dealerCard < 7))
  {
    int numAces;
    int handValue = computeHandValue( hand, n, numAces);
    if( handValue > 8 && handValue < 12)
    {
      return true;
    }
  }
  return false;
}

bool playerShouldSplit( int* hand, int n, int dealerCard)
{
  return false;
}
