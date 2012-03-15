
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

  time_t seed = 0;

  if( /*random seed*/ true)
  {
    seed = time(NULL);
    ofstream seedout;
    seedout.open("seeds.txt",ios::app);
    seedout << seed << endl;
    seedout.close();
  }
  else
  {
    // seed = 1331750333; // 1331750333 -- Long losing streak
    seed = 1331753499; // 1331753499 -- Two cropped losses

  }

  srand( seed);

  bool showHands = false;
  bool showWinnings = false;
  bool cropMajorLossesFromView = false;
  bool shadeHighCountRegions = false;

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
               + ( decks[i]>0 && decks[i]<6)
               - ( decks[i]>8 || decks[i]==0);
  }

  int numHands = 1e5;
  int hand = 0;
  int card = 0;
  int reshuffleThresh = (int)floor(0.70*numCards);
  int handDealer[21];
  int numCardsDealer = 0;
  int handPlayer[4][21];
  int numCardsPlayer = 0;
  int handPlayerValue[4] = { 0, 0, 0, 0};
  int handDealerValue = 0;
  int numAcesPlayer = 0;
  int numAcesDealer = 0;
  int losing = 0;
  int maxLosingStreak = 0;
  double wager = 1;
  int maxWager = 1;

  double wagerIncFactor=2.0;

//int maxAllowableWager = -1;
//int maxAllowableWager = 128;
  int maxAllowableWager = (int)pow(2.,30.);
//int maxAllowableWager = pow(wagerIncFactor,7);

  double* winnings;
  winnings = new double[numHands+1];
  winnings[0] = 0.0;
  double minWinnings = 0;
  double maxWinnings = 0;

  int* counts;
  counts = new int[numHands+1];

  int numPlayerBlackJacks = 0;
  int numDealerBlackJacks = 0;
  bool dealerBlackJack = false;
  bool playerBlackJack = false;

  bool betByCount = true;
  bool betMartingale = false;
  if( betByCount && betMartingale)
  {
    cout << "ERROR: Conflicting betting strategies betByCount and ";
    cout <<        "betMartingale. Turning off betByCount." << endl;
    betByCount = false;
  }

  bool allowDouble = true;
  bool allowSplit = false;
  bool doubleDown = false;
  bool splitHands = false;

  cout << "maxhands: " << numHands << endl;

  while( hand < numHands)
  {
    if( losing > maxLosingStreak) { maxLosingStreak = losing;}

    if( showHands) { cout << string(80,'=') << endl;}

    numCardsDealer = 0;
    numCardsPlayer = 0;

    numAcesPlayer = 0;
    numAcesDealer = 0;

    dealerBlackJack = false;
    playerBlackJack = false;

    doubleDown = false;
    splitHands = false;

    counts[hand] = count[card];

    if( betByCount)
    {
      wager = count[card];

#if 1
           if( wager < 5) { wager =  1;}
      else if( wager < 12) { wager =  5;}
      else                 { wager=10;}
#endif
#if 0
      if( wager < 1) { wager =  1;}
#endif
#if 0
           if( wager < 10) { wager =  0;}
      else if( wager < 17) { wager =  0;}
      else                 { wager*=10;}
#endif
#if 0
      // This one makes a winnings graph that looks like a city scape.
           if( wager <  1) { wager =  0;}
      else if( wager < 17) { wager =  1;}
      else                 { wager*=10;}
#endif
    }

    if( wager > maxAllowableWager)
    {
      wager = 1;
      losing = 0;
      if( showHands)
      {
        cout << "Cutting losses at hand " << hand << ". "
             << "(Wager exceeded $" << maxAllowableWager << ".)" << endl;
      }
    }

    if( wager > maxWager) { maxWager = wager;}

    // Deal first two cards to player and dealer.
    handPlayer[0][numCardsPlayer++] = decks[card++];
    handDealer[numCardsDealer++] = decks[card++];
    handPlayer[0][numCardsPlayer++] = decks[card++];
    handDealer[numCardsDealer++] = decks[card++];

    handPlayerValue[0] =
      computeHandValue( handPlayer[0], numCardsPlayer, numAcesPlayer);
    handDealerValue =
      computeHandValue( handDealer, numCardsDealer, numAcesDealer);

    // Check for blackjack.
    if( handPlayerValue[0] == 21)
    {
      playerBlackJack = true;
      numPlayerBlackJacks++;
    }
    else if( allowDouble
          && playerShouldDouble( handPlayer[0], numCardsPlayer, handDealer[0]))
    {
      doubleDown = true;
      wager*=2;
      handPlayer[0][numCardsPlayer++] = decks[card++];
      handPlayerValue[0] =
        computeHandValue( handPlayer[0], numCardsPlayer, numAcesPlayer);
    }
    else if( allowSplit
          && playerShouldSplit( handPlayer[0], numCardsPlayer, handDealer[0]))
    {
      splitHands = true;
      handPlayer[1][0] = handPlayer[0][1];

      numCardsPlayer = 1;
      // TODO: Finish first hand.

      numCardsPlayer = 1;
      // TODO: Finish second hand.

    }
    else
    {
      // Deal to player.
      while( playerShouldHit( handPlayer[0], numCardsPlayer, handDealer[0]))
      {
        handPlayer[0][numCardsPlayer++] = decks[card++];
        handPlayerValue[0] =
          computeHandValue( handPlayer[0], numCardsPlayer, numAcesPlayer);
      }
    }

    if( handDealerValue == 21)
    {
      dealerBlackJack = true;
      numDealerBlackJacks++;
      if( playerBlackJack)
      {
        // Don't count player's hand as a blackjack when it is pushed against a
        // dealer blackjack.
        playerBlackJack = false;
        numPlayerBlackJacks--;
      }
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
      if( handDealerValue <= 21 && handDealerValue > handPlayerValue[0])
      {
        cout << " Winner!";
      }
      cout << endl;
      displayHand( "Player", handPlayer[0], numCardsPlayer);
      if( handPlayerValue[0] <=21
       && ( handPlayerValue[0] > handDealerValue || handDealerValue > 21))
      {
        cout << " Winner!";
        cout << " ($" << wager << ")";
      }
      if( handPlayerValue[0]>21
       || ( handDealerValue <= 21 && handDealerValue > handPlayerValue[0]))
      {
        cout << " (Lost " << losing+1 << " a row.)";
      }
      cout << endl;
    }

    // Update winnings, etc.
    if( handPlayerValue[0] <= 21)
    {
      if( handPlayerValue[0] > handDealerValue || handDealerValue > 21)
      {
        winnings[hand+1] = winnings[hand] + wager;
        if( playerBlackJack)
        {
          winnings[hand+1] = winnings[hand+1] + wager/2.0;
        }
        losing = 0;
      }
      else if( handDealerValue <= 21 && handDealerValue > handPlayerValue[0])
      {
        winnings[hand+1] = winnings[hand] - wager;
        if( betMartingale) { wager*=wagerIncFactor;}
        losing++;
      }
      else if( handDealerValue <= 21)
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
      if( betMartingale) { wager*=wagerIncFactor;}
      losing++;
    }

    hand++;

    if( betMartingale && losing == 0) { wager = 1;}

    if( showWinnings)
    {
      cout << hand << " $" << winnings[hand] << endl;
    }

    // Reshuffle when necessary.
    if( card>reshuffleThresh)
    {
      if( showHands)
      {
      cout << " -- Reshuffling after card " << card << ". ("
           << card << "/"
           << numCards << "="
           << (double)card/(double)numCards
           << ")" << endl;
      }
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
                   + ( decks[i]>0 && decks[i]<6)
                   - ( decks[i]>8 || decks[i]==0);
      }

      card = 0;
      if( betByCount) { wager = 1;}
    }

    if( winnings[hand] > maxWinnings) { maxWinnings = winnings[hand];}
    if( winnings[hand] < minWinnings) { minWinnings = winnings[hand];}

  } // while( hand < numHands)

  cout << endl;
  cout << endl;
  cout << "minWinnings      : " << minWinnings       << endl;
  cout << "maxWinnings      : " << maxWinnings       << endl;
  cout << "maxWager         : " << maxWager          << endl;
  cout << "maxLosingStreak  : " << maxLosingStreak   << endl;
  cout << "maxAllowableWager: " << maxAllowableWager << endl;
  cout << "wagerIncFactor   : " << wagerIncFactor    << endl;

  ofstream fout;
  fout.open("winnings.m");
  cout << endl;
  cout << "Writing results to \"winnings.m\"." << endl;

  fout << "numHands          = " << numHands          << endl;
  fout << "minWinnings       = " << minWinnings       << endl;
  fout << "maxWinnings       = " << maxWinnings       << endl;
  fout << "maxWager          = " << maxWager          << endl;
  fout << "maxLosingStreak   = " << maxLosingStreak   << endl;
  fout << "maxAllowableWager = " << maxAllowableWager << endl;
  fout << "wagerIncFactor    = " << wagerIncFactor    << endl;
  fout << "x = [" << endl;
  for( i=0; i<=numHands; i++)
  {
    fout << winnings[i] << endl;
  }
  fout << "];" << endl;
  fout << "k = [" << endl;
  for( i=0; i<=numHands; i++)
  {
    fout << counts[i] << endl;
  }
  fout << "];" << endl;
  fout << "plot(x);" << endl;
  fout << "xlabel('hands');" << endl;
  fout << "ylabel('winnings');" << endl;

  fout << "kmin = min(k);" << endl;
  fout << "kmax = max(k);" << endl;

  if( shadeHighCountRegions)
  {
    fout << "hold on;" << endl;
    fout << "for i=1:numHands" << endl;
    fout << "  hnd=line( i*[1 1], [ 0 x(i)]);" << endl;
    fout << "  set(hnd,'color', ((k(i)-kmin)/(kmax-kmin))*[ 1 0.0 0.0] + ((kmax-k(i))/(kmax-kmin))*[ 0.0 0.0 1]);" << endl;
    fout << "end" << endl;
    fout << "hnd=plot(x);" << endl;
    fout << "set(hnd,'color',[0 0 0]);" << endl;
    fout << "set(hnd,'linewidth',1.5);" << endl;
    fout << "hold off;" << endl;
  }

  if( cropMajorLossesFromView)
  {
    fout << "ymin = max(-maxWinnings,minWinnings);" << endl;
  }
  else
  {
    fout << "ymin = minWinnings;" << endl;
  }

  fout << "yrange = maxWinnings - ymin;" << endl;
  fout << "axis([ 0-0.05*numHands ";
  fout <<        "1.05*numHands ";
  fout <<        "ymin-0.05*yrange ";
  fout <<        "maxWinnings+0.05*yrange]);" << endl;

  fout << "xlim = get(gca,'xlim');" << endl;
  fout << "ylim = get(gca,'ylim');" << endl;

  fout << "if( ylim(1) > minWinnings)" << endl;
  fout << "  xmin = find(x<ylim(1));" << endl;
  fout << "  hold on;" << endl;
  fout << "  plot( xmin, ylim(1)*ones(size(xmin)), 'r*');" << endl;
  fout << "  hold off;" << endl;
  fout << "end" << endl;

  fout << "dx = (xlim(2)-xlim(1))/40;" << endl;
  fout << "dy = (ylim(2)-ylim(1))/30;" << endl;
  fout << "x0 = dx;" << endl;

  fout << "y0 = ylim(2)-dy;" << endl;

  fout << "hnd=text(x0,y0,sprintf('numHands = %d',numHands)); y0 = y0 - dy;" << endl;
  fout << "set(hnd,'color', 0.5*[ 1 1 1]);" << endl;

  fout << "hnd=text(x0,y0,sprintf('minWinnings = %3.2e',minWinnings)); y0 = y0 - dy;" << endl;
  fout << "set(hnd,'color', 0.5*[ 1 1 1]);" << endl;

  fout << "hnd=text(x0,y0,sprintf('maxWinnings = %3.2e',maxWinnings)); y0 = y0 - dy;" << endl;
  fout << "set(hnd,'color', 0.5*[ 1 1 1]);" << endl;

  fout << "hnd=text(x0,y0,sprintf('maxWager = %d',maxWager)); y0 = y0 - dy;" << endl;
  fout << "set(hnd,'color', 0.5*[ 1 1 1]);" << endl;

  fout << "hnd=text(x0,y0,sprintf('maxLosingStreak = %d',maxLosingStreak)); y0 = y0 - dy;" << endl;
  fout << "set(hnd,'color', 0.5*[ 1 1 1]);" << endl;

  fout << "hnd=text(x0,y0,sprintf('maxAllowableWager = %d',maxAllowableWager)); y0 = y0 - dy;" << endl;
  fout << "set(hnd,'color', 0.5*[ 1 1 1]);" << endl;

  fout << "hnd=text(x0,y0,sprintf('wagerIncFactor = %3.2f',wagerIncFactor)); y0 = y0 - dy;" << endl;
  fout << "set(hnd,'color', 0.5*[ 1 1 1]);" << endl;

  fout << "hnd=text(x0,y0,sprintf('maxCount = %d',kmax)); y0 = y0 - dy;" << endl;
  fout << "set(hnd,'color', 0.5*[ 1 1 1]);" << endl;

  fout << "hnd=text(x0,y0,sprintf('minCount = %d',kmin)); y0 = y0 - dy;" << endl;
  fout << "set(hnd,'color', 0.5*[ 1 1 1]);" << endl;

  fout << "" << endl;
  fout << "" << endl;
  fout << "" << endl;
  fout << "" << endl;
  fout << "" << endl;
  fout << "" << endl;
  fout << "" << endl;
  fout << "" << endl;
  fout << "" << endl;
  fout << "" << endl;
  fout << "" << endl;
  fout << "" << endl;
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
      case  9: cout << " 0"; break;
      case 10: cout << " J"; break;
      case 11: cout << " Q"; break;
      case 12: cout << " K"; break;
      default: cout << " " << hand[i]+1;
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
      if( hand[i]>8) { val+=10;}
      else { val+=(hand[i]+1);}
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
         ( handValue > 12 && ( dealerCard > 5 || !dealerCard))
      ||
         ( handValue==12 && ( dealerCard > 5 || dealerCard < 3 ))
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
  if( ( dealerCard > 1 || dealerCard < 6))
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
