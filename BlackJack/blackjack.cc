
#include "blackjack.h"

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
using namespace std;

namespace dthorne0_blackjack
{

//                                 R U L E S
//##############################################################################
Rules::Rules( int numDecks
            , float maxPenetrationRatio
            , float blackjackPayoutFactor
            )
{
  m_numDecks = numDecks;
  m_maxPenetrationRatio = maxPenetrationRatio;
  m_blackjackPayoutFactor = blackjackPayoutFactor;
}

//                                  S H O E
//##############################################################################
Shoe::Shoe( const Rules& rules)
: m_rules(rules)
{
  m_numCards = 52*m_rules.numDecks();
  m_cards = new int[m_numCards];
  shuffle();
  m_curCard = 0;
  m_betweenHands = true;
}

Shoe::~Shoe()
{
  delete [] m_cards;
}

Shoe::Shoe( const Shoe& src)
: m_rules(src.m_rules)
{
  cerr
    << __FILE__ << " " << __LINE__ << ", "
    << "Shoe::Shoe( const Shoe& src) -- Pending! (Exiting.)"
    << endl;
  exit(1);
}

void Shoe::operator=( const Shoe& rhs)
{
  cerr
    << __FILE__ << " " << __LINE__ << ", "
    << "Shoe::operator=( const Shoe& rhs) -- Pending! (Exiting.)"
    << endl;
  exit(1);
}

bool Shoe::shouldReshuffle() const
{
  return m_curCard > m_numCards*m_rules.maxPenetrationRatio();
}

void Shoe::shuffle()
{
  int i;

  if( verbose())
  {
    cout << __FILE__ << __LINE__ << " "
         << "Initialize m_numCards/13 decks in the shoe."
         << endl;
  }
  for( i=0; i<m_numCards; i++)
  {
    if( (i%13)+1 < 10)
    {
      m_cards[i] = (i%13)+1;
    }
    else
    {
      m_cards[i] = 0;
    }
  }

  if( true)
  {
    if( verbose())
    {
      cout << __FILE__ << " " << __LINE__ << " "
           << "Shuffle the m_numCards/13 decks together."
           << endl;
    }
    int j, temp;
    if( false)
    {
      for( i=0; i<m_numCards*m_numCards; i++)
      {
        j = (int)floor(m_numCards*( float(rand())/float(RAND_MAX)));
        temp = m_cards[0];
        m_cards[0] = m_cards[j];
        m_cards[j] = temp;
      }
    }
    else
    {
      for( i=m_numCards-1; i>0; i--)
      {
        j = (int)floor(i*( float(rand())/float(RAND_MAX)));
        temp = m_cards[i];
        m_cards[i] = m_cards[j];
        m_cards[j] = temp;
      }
    }
  }
  else
  {
    cout << __FILE__ << " " << __LINE__ << " "
         << "Skipping shuffle."
         << endl;
  }
}

int Shoe::deal()
{
  if( m_betweenHands && shouldReshuffle()) { shuffle(); m_curCard=0;}
  m_betweenHands = false;
  return m_cards[m_curCard++];
}

void Shoe::newHand()
{
  m_betweenHands = true;
}

void Shoe::display( std::ostream& o) const
{
  int i;
  for( i=0; i<m_numCards; i++)
  {
    if( i!=m_curCard)
    {
      o << m_cards[i];
    }
    else
    {
      o << " [ " << m_cards[i];
    }
  }
  o << "]";
}

//                                D E A L E R
//##############################################################################
Dealer::Dealer( const Rules& rules)
: m_rules(rules)
{
  m_hand = new int[21];
  m_numCards = 0;
  m_handValue = 0;
  m_numElevens = 0;
  m_handSoft = false;
}

void Dealer::nextCard( const int card)
{
  m_hand[m_numCards++] = card;
  if( card==1)
  {
    m_handValue+=11;
    m_numElevens++;
  }
  else if( card==0)
  {
    m_handValue+=10;
  }
  else
  {
    m_handValue+=card;
  }

  while( m_handValue > 21 && m_numElevens > 0)
  {
    m_handValue-=10;
    m_numElevens--;
  }

  if( m_numElevens>0) { m_handSoft=true;}
  else { m_handSoft=false;}
}

bool Dealer::blackjack()
{
  if( m_numCards==2)
  {
    if( m_hand[0]==1 && !m_hand[1])
    {
      return m_blackjack=true;
    }
    if( m_hand[1]==1 && !m_hand[0])
    {
      return m_blackjack=true;
    }
  }
  return false;
}

int Dealer::upCard() const
{
  return m_hand[0];
}

bool Dealer::busts()
{
  if( m_handValue > 21)
  {
    return m_busted=true;
  }
  return false;
}

bool Dealer::hits() const
{
  return m_handValue<17;
}

void Dealer::newHand()
{
  m_numCards = 0;
  m_handValue = 0;
  m_numElevens = 0;
  m_handSoft = false;
  m_busted = false;
  m_blackjack = false;
}

void Dealer::display( std::ostream& o) const
{
  int i;
  o << "Dealer hand: [";
  for( i=0; i<m_numCards; i++)
  {
    o << " ";
    switch( m_hand[i])
    {
      case 0: // Tens/Faces
        o << "T";
        break;
      case 1: // Aces
        o << "A";
        break;
      default: // Others
        o << m_hand[i];
        break;
    }
  }
  o << "]";
  o << " ";
  if( m_handValue < 21 && m_handSoft)
  {
    o << "Soft ";
  }
  o << m_handValue;
  if( m_busted)
  {
    o << " -- BUST";
  }
}

//                                P L A Y E R
//##############################################################################
Player::Player( const Rules& rules)
: m_rules(rules)
{
  m_hand = new int[21];
  m_numCards = 0;
  m_handValue = 0;
  m_numElevens = 0;
  m_handSoft = false;
  m_wonLastHand = true;
  m_pushLastHand = false;
  m_initialWager = 5;
  m_currentWager = m_initialWager;
  m_initialBankRoll = 200;
  m_bankRoll = m_initialBankRoll;
  m_numWins = 0;
  m_numLosses = 0;
  m_numPushes = 0;
  m_numBlackjacks = 0;

  m_numHandsToBreakBank = 0;
  m_minHandsToBreakBank = 999999;
  m_maxHandsToBreakBank = 0;
  m_numBankBreaks = 0;
  m_maxBankRoll = m_initialBankRoll;
  m_sumBankRollPeaks = 0;
  m_peakHand = 0;
  m_sumPeakHands = 0;
  m_lengthLosingStreak = 0;

  m_maxNumHandsToRecord = 128*128;
  m_peakHandHistogram = new int[m_maxNumHandsToRecord];
  m_peakBankHistogram = new int[m_maxNumHandsToRecord];
  m_sumBankRollPerHand = new float[m_maxNumHandsToRecord];
  m_countBankRollUp = new int[m_maxNumHandsToRecord];
  m_countBankRollDown = new int[m_maxNumHandsToRecord];
  m_handOrdinalCounts = new int[m_maxNumHandsToRecord];
  m_countLosingStreaks = new int[m_maxNumHandsToRecord];
  m_countLosingStreaksOfGivenLength = new int[m_maxNumHandsToRecord];
  m_lengthLosingStreakToTrack = 6;
  int i;
  for( i=0; i<m_maxNumHandsToRecord; i++)
  {
    m_peakHandHistogram[i] = 0;
    m_peakBankHistogram[i] = 0;
    m_sumBankRollPerHand[i] = 0;
    m_countBankRollUp[i] = 0;
    m_countBankRollDown[i] = 0;
    m_handOrdinalCounts[i] = 0;
    m_countLosingStreaks[i] = 0;
    m_countLosingStreaksOfGivenLength[i] = 0;
  }
}

Player::Player( const Player& src)
: m_rules(src.m_rules)
{
  cerr
    << __FILE__ << " " << __LINE__ << ", "
    << "Player::Player( const Player& src) -- Pending! (Exiting.)"
    << endl;
  exit(1);
}

void Player::operator=( const Player& rhs)
{
  cerr
    << __FILE__ << " " << __LINE__ << ", "
    << "Player::operator=( const Player& rhs) -- Pending! (Exiting.)"
    << endl;
  exit(1);
}

Player::~Player()
{
  delete [] m_hand;
}

void Player::nextCard( const int card)
{
  m_hand[m_numCards++] = card;
  if( card==1)
  {
    m_handValue+=11;
    m_numElevens++;
  }
  else if( card==0)
  {
    m_handValue+=10;
  }
  else
  {
    m_handValue+=card;
  }

  while( m_handValue > 21 && m_numElevens > 0)
  {
    m_handValue-=10;
    m_numElevens--;
  }

  if( m_numElevens>0) { m_handSoft=true;}
  else { m_handSoft=false;}
}

float Player::bets()
{
  if( m_wonLastHand)
  {
    m_currentWager = m_initialWager;
  }
  else if( !m_pushLastHand)
  {
    if( true)
    {
      m_currentWager*=2;
      //m_currentWager++;
      if( m_currentWager > m_bankRoll && m_bankRoll > 0)
      {
        m_currentWager = m_bankRoll;
      }
      //if( m_currentWager > m_bankRoll) { m_currentWager=1;}
      //if( m_currentWager > m_bankRoll) { m_currentWager=m_bankRoll/2.0;}
    }
    else
    {
      // Assuming m_initialWager is one.
      if( m_initialWager != 1)
      {
        cout << __FILE__ << " " << __LINE__ << " "
             << "WARNING: m_initialWager should be 1 for this betting system."
             << endl;
      }
      switch( (int)m_currentWager)
      {
        case 1: m_currentWager = 2; break;
        case 2: m_currentWager = 5; break;
        case 5: m_currentWager = 11; break;
        case 11: m_currentWager = 24; break;
        case 24: m_currentWager = 52; break;
        case 52: m_currentWager = 105; break;
        default: m_currentWager = 200; break;
      }
      if( m_currentWager > m_bankRoll) { m_currentWager=m_bankRoll;}
    }
  }

  if( m_currentWager > m_bankRoll)
  {
    //cout << __FILE__ << " " << __LINE__ << " "
    //     << "wager ($" << m_currentWager
    //     << ") is larger than bank roll ($" << m_bankRoll
    //     << ")"
    //     << endl;
    m_numBankBreaks++;
    if( m_numHandsToBreakBank < m_minHandsToBreakBank)
    {
      m_minHandsToBreakBank = m_numHandsToBreakBank;
    }
    if( m_numHandsToBreakBank > m_maxHandsToBreakBank)
    {
      m_maxHandsToBreakBank = m_numHandsToBreakBank;
    }
    m_numHandsToBreakBank = 0;
    m_currentWager = m_initialWager;
    m_bankRoll = m_initialBankRoll;
    m_sumBankRollPeaks += m_maxBankRoll;
    if( m_peakHand < m_maxNumHandsToRecord)
    {
      m_peakHandHistogram[ m_peakHand]++;
      m_peakBankHistogram[ m_peakHand]+=m_maxBankRoll;
    }
    m_sumPeakHands += m_peakHand;

    m_maxBankRoll = m_initialBankRoll;
    m_peakHand = 0;
  }
  else
  {
    m_numHandsToBreakBank++;
  }

  return m_currentWager;
}

bool Player::blackjack()
{
  if( m_numCards==2)
  {
    if( m_hand[0]==1 && !m_hand[1])
    {
      if( !m_blackjack) { m_numBlackjacks++;}
      m_pushLastHand = false;
      return m_blackjack=true;
    }
    if( m_hand[1]==1 && !m_hand[0])
    {
      if( !m_blackjack) { m_numBlackjacks++;}
      m_pushLastHand = false;
      return m_blackjack=true;
    }
  }
  return false;
}

void Player::push()
{
  m_pushLastHand = true;
  if( m_numHandsToBreakBank < m_maxNumHandsToRecord)
  {
    if( m_bankRoll < 0)
    {
      cout << __FILE__ << " " << __LINE__ << " "
           << "Bank roll went negative!"
           << endl;
      exit(1);
    }
    m_sumBankRollPerHand[m_numHandsToBreakBank] += m_bankRoll;
    m_handOrdinalCounts[m_numHandsToBreakBank]++;
    if( m_bankRoll > m_initialBankRoll)
    {
      m_countBankRollUp[m_numHandsToBreakBank]++;
    }
    else if( m_bankRoll < m_initialBankRoll)
    {
      m_countBankRollDown[m_numHandsToBreakBank]++;
    }
  }
  m_numPushes++;
}

void Player::loses( float wager)
{
  m_bankRoll -= wager;
  if( m_numHandsToBreakBank < m_maxNumHandsToRecord)
  {
    if( m_bankRoll < 0)
    {
      cout << __FILE__ << " " << __LINE__ << " "
           << "Bank roll went negative!"
           << endl;
      exit(1);
    }
    m_sumBankRollPerHand[m_numHandsToBreakBank] += m_bankRoll;
    m_handOrdinalCounts[m_numHandsToBreakBank]++;
    if( m_bankRoll > m_initialBankRoll)
    {
      m_countBankRollUp[m_numHandsToBreakBank]++;
    }
    else if( m_bankRoll < m_initialBankRoll)
    {
      m_countBankRollDown[m_numHandsToBreakBank]++;
    }
  }
  m_wonLastHand = false;
  m_pushLastHand = false;
  m_numLosses++;
  m_lengthLosingStreak++;
}

void Player::wins( float payout)
{
  m_bankRoll += payout;
  if( m_numHandsToBreakBank < m_maxNumHandsToRecord)
  {
    if( m_bankRoll < 0)
    {
      cout << __FILE__ << " " << __LINE__ << " "
           << "Bank roll went negative!"
           << endl;
      exit(1);
    }
    m_sumBankRollPerHand[m_numHandsToBreakBank] += m_bankRoll;
    m_handOrdinalCounts[m_numHandsToBreakBank]++;
    if( m_bankRoll > m_initialBankRoll)
    {
      m_countBankRollUp[m_numHandsToBreakBank]++;
    }
    else if( m_bankRoll < m_initialBankRoll)
    {
      m_countBankRollDown[m_numHandsToBreakBank]++;
    }
    if(m_lengthLosingStreak == m_lengthLosingStreakToTrack)
    {
      m_countLosingStreaksOfGivenLength[m_numHandsToBreakBank]++;
    }
  }
  if( m_bankRoll > m_maxBankRoll)
  {
    m_maxBankRoll = m_bankRoll;
    m_peakHand = m_numHandsToBreakBank;
  }
  m_countLosingStreaks[m_lengthLosingStreak]++;
  m_lengthLosingStreak = 0;
  m_wonLastHand = true;
  m_pushLastHand = false;
  m_numWins++;
}

bool Player::busts()
{
  if( m_handValue > 21)
  {
    return m_busted=true;
  }
  return false;
}

bool Player::hits( int dealerUpCard) const
{
  // TODO: Basic Strategy

  // "Wizard's strategy" (http://wizardofodds.com/blackjack)
  // TODO: Implement doubling and splitting.
  if( dealerUpCard>=2 && dealerUpCard<=6)
  {
    if( m_handSoft)
    {
      if( m_handValue>=13 && m_handValue<=15)
      {
        return true;
      }
      else if( m_handValue>=16 && m_handValue<=18)
      {
        return true; // TODO: Should double.
      }
      else if( m_handValue>=19 && m_handValue<=21)
      {
        return false;
      }
      else if( m_handValue==12)
      {
        return false; // TODO: Should split.
      }
      else
      {
        cout << __FILE__ << " " << __LINE__ << " "
             << "Unhandled case."
             << endl;
        exit(1);
      }
    }
    else
    {
      if( m_handValue>=4 && m_handValue<=8)
      {
        return true;
        // TODO: Split 2 2, 3 3
      }
      else if( m_handValue>=9 && m_handValue<=11)
      {
        return true; // TODO: Should double.
      }
      else if( m_handValue>=12 && m_handValue<=21)
      {
        return false;
        // TODO: Slpit 6 6, 7 7, 8 8, 9 9
      }
      else
      {
        cout << __FILE__ << " " << __LINE__ << " "
             << "Unhandled case."
             << endl;
        exit(1);
      }
    }
  }
  else
  {
    if( m_handSoft)
    {
      if( m_handValue>=13 && m_handValue<=15)
      {
        return true;
      }
      else if( m_handValue>=16 && m_handValue<=18)
      {
        return true;
      }
      else if( m_handValue>=19 && m_handValue<=21)
      {
        return false;
      }
      else if( m_handValue==12)
      {
        return true; // TODO: Should split.
      }
      else
      {
        cout << __FILE__ << " " << __LINE__ << " "
             << "Unhandled case."
             << endl;
        exit(1);
      }
    }
    else
    {
      if( m_handValue>=4 && m_handValue<=9)
      {
        return true;
      }
      else if( m_handValue>=10 && m_handValue<=11)
      {
        return true; // TODO: Should double (unless up card is larger).
      }
      else if( m_handValue>=12 && m_handValue<=16)
      {
        return true;
        // TODO: Split 8 8.
      }
      else
      {
        return false;
      }
    }
  }

  return m_handValue<17;
}

void Player::newHand()
{
  m_numCards = 0;
  m_handValue = 0;
  m_numElevens = 0;
  m_handSoft = false;
  m_busted = false;
  m_blackjack = false;
}

void Player::display( std::ostream& o) const
{
  int i;
  o << "Player hand: [";
  for( i=0; i<m_numCards; i++)
  {
    o << " ";
    switch( m_hand[i])
    {
      case 0: // Tens/Faces
        o << "T";
        break;
      case 1: // Aces
        o << "A";
        break;
      default: // Others
        o << m_hand[i];
        break;
    }
  }
  o << "]";
  o << " ";
  if( !m_pushLastHand && m_blackjack)
  {
    o << "Blackjack!";
  }
  else
  {
    if( m_handSoft)
    {
      o << "Soft ";
    }
    o << m_handValue;
    o << " -- ";
    if( m_wonLastHand)
    {
      o << "WON!";
    }
    else if( m_pushLastHand)
    {
      o << "PUSH";
    }
    else if( m_busted)
    {
      o << "BUST";
    }
    else
    {
      o << "LOST";
    }
  }
  o << " -- ";
  o << "New bankroll: ";
  o << m_bankRoll;
}

//                                 T A B L E
//##############################################################################
Table::Table( const Rules& rules)
: m_rules(rules)
{
  m_shoe = new Shoe(m_rules);
  m_dealer = new Dealer(m_rules);
  m_player = new Player(m_rules);

}

void Table::play( const int numHands)
{
  int wager = 0;
  int n;

  for( n=0; n<numHands; n++)
  {
    m_shoe->newHand();
    m_player->newHand();
    m_dealer->newHand();

    wager = m_player->bets();

    m_player->nextCard( m_shoe->deal());
    if( verbose())
    {
      cout << __FILE__ << " " << __LINE__ << " "
           << "Player dealt " << m_player->curCard()
           << " (hand value = "
           << m_player->handValue()
           << ")"
           << endl;
    }

    m_player->nextCard( m_shoe->deal());
    if( verbose())
    {
      cout << __FILE__ << " " << __LINE__ << " "
           << "Player dealt " << m_player->curCard()
           << " (hand value = "
           << m_player->handValue()
           << ")"
           << endl;
    }

    m_dealer->nextCard( m_shoe->deal());
    if( verbose())
    {
      cout << __FILE__ << " " << __LINE__ << " "
           << "Dealer dealt " << m_dealer->curCard()
           << " (hand value = "
           << m_dealer->handValue()
           << ")"
           << endl;
    }

    m_dealer->nextCard( m_shoe->deal());
    if( verbose())
    {
      cout << __FILE__ << " " << __LINE__ << " "
           << "Dealer dealt " << m_dealer->curCard()
           << " (hand value = "
           << m_dealer->handValue()
           << ")"
           << endl;
    }

    // TODO: surrender option
    // TODO: insurance option
    // TODO: split option
    // TODO: double option

    if( m_dealer->blackjack())
    {
      if( verbose())
      {
        cout << __FILE__ << " " << __LINE__ << " "
             << "Dealer Blackjack."
             << endl;
      }

      if( m_player->blackjack())
      {
        if( verbose())
        {
          cout << __FILE__ << " " << __LINE__ << " "
               << "Player Blackjack, too, so push."
               << endl;
        }
        m_player->push();
      }
      else
      {
        if( verbose())
        {
          cout << __FILE__ << " " << __LINE__ << " "
               << "Player loses."
               << endl;
        }
        m_player->loses( wager);
      }
    }
    else
    {
      if( m_player->blackjack())
      {
        if( verbose())
        {
          cout << __FILE__ << " " << __LINE__ << " "
               << "Player blackjack!"
               << endl;
        }
        m_player->wins( m_rules.blackjackPayout(wager));
      }
      else
      {
        while( m_player->hits( m_dealer->upCard()))
        {
          if( verbose())
          {
            cout << __FILE__ << " " << __LINE__ << " "
                 << "Player hits."
                 << endl;
          }
          m_player->nextCard( m_shoe->deal());
          if( verbose())
          {
            cout << __FILE__ << " " << __LINE__ << " "
                 << "Player dealt " << m_player->curCard()
                 << " (hand value = "
                 << m_player->handValue()
                 << ")"
                 << endl;
          }
        }

        if( !m_player->busts())
        {
          while( m_dealer->hits())
          {
            if( verbose())
            {
              cout << __FILE__ << " " << __LINE__ << " "
                   << "Dealer hits."
                   << endl;
            }
            m_dealer->nextCard( m_shoe->deal());
            if( verbose())
            {
              cout << __FILE__ << " " << __LINE__ << " "
                   << "Dealer dealt " << m_dealer->curCard()
                   << " (hand value = "
                   << m_dealer->handValue()
                   << ")"
                   << endl;
            }
          }

          if( !m_dealer->busts())
          {
            if( *m_player > *m_dealer)
            {
              if( verbose())
              {
                cout << __FILE__ << " " << __LINE__ << " "
                     << "Player wins!"
                     << endl;
              }
              m_player->wins( wager);
            }
            else if( *m_player != *m_dealer)
            {
              if( verbose())
              {
                cout << __FILE__ << " " << __LINE__ << " "
                     << "Player loses."
                     << endl;
              }
              m_player->loses( wager);
            }
            else
            {
              if( verbose())
              {
                cout << __FILE__ << " " << __LINE__ << " "
                     << "Push."
                     << endl;
              }
              m_player->push();
            }
          }
          else
          {
            if( verbose())
            {
              cout << __FILE__ << " " << __LINE__ << " "
                   << "Dealer busts. Player wins!"
                   << endl;
            }
            m_player->wins( wager);
          }
        }
        else
        {
          if( verbose())
          {
            cout << __FILE__ << " " << __LINE__ << " "
                 << "Player loses."
                 << endl;
          }
          m_player->loses( wager);
        }
      }
    }
    if( displayHands())
    {
      cout << endl;
      cout << *m_dealer << endl;
      cout << "Player wager = " << wager << endl;
      cout << *m_player << endl;
    }
    if( displayShoe())
    {
      cout << *m_shoe << endl;
    }
  }
}

void Table::display( ostream& o) const
{
  o << "Results:" << endl;
  o << "  Player won " << m_player->numWins() << " hands"
    << " (" << m_player->numBlackjacks() << " blackjacks)." << endl;
  o << "  Player lost " << m_player->numLosses() << " hands." << endl;
  o << "  Pushed on " << m_player->numPushes() << " hands." << endl;
  o << endl;
  float totalHands = m_player->numWins()
                   + m_player->numLosses()
                   + m_player->numPushes();
  if( totalHands > 0)
  {
    o << "  Ratio wins  : " << m_player->numWins()/totalHands << endl;
    o << "  Ratio losses: " << m_player->numLosses()/totalHands << endl;
    o << "  Ratio pushes: " << m_player->numPushes()/totalHands << endl;
    o << "  Ratio blackjacks: " << m_player->numBlackjacks()/totalHands << endl;
    o << endl;
    o << "  Average run before going broke: "
      << (float)totalHands/m_player->numBankBreaks()
      << " hands."
      << endl;
    o << "  Minimum run before going broke: "
      << m_player->minHandsToBreakBank()
      << " hands."
      << endl;
    o << "  Maximum run before going broke: "
      << m_player->maxHandsToBreakBank()
      << " hands."
      << endl;
    o << endl;
    o << "  Average peak hand     : "
      << m_player->sumPeakHands()/m_player->numBankBreaks()
      << " hands."
      << endl;
    o << "  Average bank roll peak: $"
      << m_player->sumBankRollPeaks() / m_player->numBankBreaks()
      << endl;
    if( true)
    {
      o
/*1*/   << "% 1 " << "peakHandHistogram"
/*2*/   << "% 2 " << "peakBankHistogram"
/*3*/   << "% 3 " << "sumBankRollPerHand"
/*4*/   << "% 4 " << "handOrdinalCounts"
/*5*/   << "% 5 " << "countLosingStreaks"
/*6*/   << "% 6 " << "countBankRollUp"
/*7*/   << "% 7 " << "countBankRollDown"
/*8*/   << "% 8 " << "countLosingStreaksOfGivenLength"
        << ";"
        << endl;

      o << "x = [" << endl;
      int i;
      for( i=0; i<m_player->maxNumHandsToRecord(); i++)
      {
        o << setw(6) << i
/*1*/     << ": " << m_player->peakHandHistogram(i)
/*2*/     << "  " << m_player->peakBankHistogram(i)
/*3*/     << " " << m_player->sumBankRollPerHand(i)
/*4*/     << " " << m_player->handOrdinalCounts(i)
/*5*/     << " " << m_player->countLosingStreaks(i)
/*6*/     << " " << m_player->countBankRollUp(i)
/*7*/     << " " << m_player->countBankRollDown(i)
/*8*/     << " " << m_player->countLosingStreaksOfGivenLength(i)
          << ";"
          << endl;
      }
      o << "];" << endl;
      o << "plot(x(:,6)./x(:,4)); % ratio of wins w.r.t. number of hands played"
        << endl;
    }
  }
  else
  {
    o << "Initial bank roll: $" << m_player->initialBankRoll() << endl;
    o << "Initial wager: $" << m_player->initialWager() << endl;
  }
}

//                           N O N - M E M B E R S
//##############################################################################
ostream& operator<<( ostream& o, const Table& x)
{
  x.display(o);
  return o;
}

ostream& operator<<( ostream& o, const Player& x)
{
  x.display(o);
  return o;
}

ostream& operator<<( ostream& o, const Dealer& x)
{
  x.display(o);
  return o;
}

ostream& operator<<( ostream& o, const Shoe& x)
{
  x.display(o);
  return o;
}

bool operator>( const Player& player, const Dealer& dealer)
{
  // Assuming neither player nor dealer busted.
  return player.handValue() > dealer.handValue();
}

bool operator!=( const Player& player, const Dealer& dealer)
{
  return player.handValue() != dealer.handValue();
}

}
