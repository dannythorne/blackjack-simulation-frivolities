#ifndef BLACKJACK_H
#define BLACKJACK_H

#include <iostream>

namespace dthorne0_blackjack
{
//##############################################################################
class Blackjack
{
public:
  Blackjack()
  {
    m_verbose = false;
    m_displayHands = false;
    m_displayShoe = false;
  }
  bool verbose() const { return m_verbose;}
  void toggle_verbosity() { m_verbose = !m_verbose;}
  bool displayHands() const { return m_displayHands;}
  void toggle_displayHands() { m_displayHands = !m_displayHands;}
  bool displayShoe() const { return m_displayShoe;}
  void toggle_displayShoe() { m_displayShoe = !m_displayShoe;}
protected:
  bool m_verbose;
  bool m_displayHands;
  bool m_displayShoe;
};

//##############################################################################
class Rules : public Blackjack
{
public:
  Rules( int numDecks=4
       , float maxPenetrationRatio=0.7
       , float blackjackPayoutFactor=1.5
       );

  int numDecks() const { return m_numDecks;}

  float blackjackPayoutFactor() const { return m_blackjackPayoutFactor;}
  float blackjackPayout( float wager) const
  {return m_blackjackPayoutFactor*wager;}

  float maxPenetrationRatio() const { return m_maxPenetrationRatio;}

private:
  int m_numDecks;
  float m_blackjackPayoutFactor;
  float m_maxPenetrationRatio;
};

//##############################################################################
class Shoe : public Blackjack
{
public:
  Shoe( const Rules& rules);
  ~Shoe();
  Shoe( const Shoe& src);
  void operator=( const Shoe& rhs);
  bool shouldReshuffle() const;
  void shuffle();
  int deal();
  void newHand();
  void display( std::ostream& o) const;
private:
  const Rules& m_rules;
  int m_numCards;
  int m_curCard;
  int* m_cards;
  bool m_betweenHands;

};

//##############################################################################
class Dealer : public Blackjack
{
public:
  Dealer( const Rules& rules);

  int curCard() const { return m_hand[m_numCards-1];}
  void nextCard( const int card);

  bool blackjack();
  int upCard() const;
  bool busts();
  bool hits() const;

  int handValue() const { return m_handValue;}

  void newHand();

  void display( std::ostream& o) const;

private:
  const Rules& m_rules;
  int* m_hand;
  int m_numCards;
  int m_handValue;
  int m_numElevens;
  int m_handSoft;
  int m_busted;
  int m_blackjack;
};

//##############################################################################
class Player : public Blackjack
{
public:
  Player( const Rules& rules);
  ~Player();
  Player( const Player& x);
  void operator=( const Player& x);

  int curCard() const { return m_hand[m_numCards-1];}
  void nextCard( const int card);

  float bets();

  bool blackjack();
  void push();
  void loses( float wager);
  void wins( float payout);
  bool busts();
  bool hits( int dealerUpCard) const;

  int handValue() const { return m_handValue;}

  void newHand();

  void display( std::ostream& o) const;

  int initialWager() const { return m_initialWager;}
  int initialBankRoll() const { return m_initialBankRoll;}

  int numWins() const { return m_numWins;}
  int numLosses() const { return m_numLosses;}
  int numBlackjacks() const { return m_numBlackjacks;}
  int numPushes() const { return m_numPushes;}

  int numBankBreaks() const { return m_numBankBreaks;}
  int minHandsToBreakBank() const { return m_minHandsToBreakBank;}
  int maxHandsToBreakBank() const { return m_maxHandsToBreakBank;}
  float sumBankRollPeaks() const { return m_sumBankRollPeaks;}
  float sumPeakHands() const { return m_sumPeakHands;}

  int maxNumHandsToRecord() const { return m_maxNumHandsToRecord;}
  int peakHandHistogram( const int i) const { return m_peakHandHistogram[i];}
  int peakBankHistogram( const int i) const { return m_peakBankHistogram[i];}
  float sumBankRollPerHand( const int i) const
  { return m_sumBankRollPerHand[i];}
  int countBankRollUp( const int i) const { return m_countBankRollUp[i];}
  int countBankRollDown( const int i) const { return m_countBankRollDown[i];}
  int handOrdinalCounts( const int i) const
  { return m_handOrdinalCounts[i];}
  int countLosingStreaks( const int i) const
  { return m_countLosingStreaks[i];}
  int countLosingStreaksOfGivenLength( const int i) const
  { return m_countLosingStreaksOfGivenLength[i];}
  int lengthLosingStreakToTrack() const { return m_lengthLosingStreakToTrack;}

private:
  const Rules& m_rules;

  int* m_hand;
  int m_numCards;

  int m_handValue;
  int m_numElevens;
  bool m_handSoft;
  bool m_wonLastHand;
  bool m_pushLastHand;
  int m_busted;
  int m_blackjack;

  float m_initialWager;
  float m_currentWager;
  float m_initialBankRoll;
  float m_bankRoll;

  int m_numWins;
  int m_numLosses;
  int m_numPushes;
  int m_numBlackjacks;

  int m_curHand;
  int m_minHandsToBreakBank;
  int m_maxHandsToBreakBank;
  int m_numBankBreaks;
  float m_maxBankRoll;
  float m_sumBankRollPeaks;
  int m_peakHand;
  float m_sumPeakHands;
  int m_maxNumHandsToRecord;
  int* m_peakHandHistogram;
  int* m_peakBankHistogram;
  float* m_sumBankRollPerHand;
  int* m_countBankRollUp;
  int* m_countBankRollDown;
  int* m_handOrdinalCounts;

  int m_lengthLosingStreak;
  int* m_countLosingStreaks;
  int* m_countLosingStreaksOfGivenLength;
  int m_lengthLosingStreakToTrack;

};

//##############################################################################
class Table : public Blackjack
{
public:
  Table( const Rules& rules);

  void toggle_verbosity()
  {
    Blackjack::toggle_verbosity();
    m_shoe->toggle_verbosity();
    m_dealer->toggle_verbosity();
    m_player->toggle_verbosity();
  }

  void play( const int numHands=1);

  void display( std::ostream& o) const;

private:

  const Rules& m_rules;
  Shoe* m_shoe;
  Dealer* m_dealer;
  Player* m_player;

};

//##############################################################################
std::ostream& operator<<( std::ostream& o, const Table& x);
std::ostream& operator<<( std::ostream& o, const Player& x);
std::ostream& operator<<( std::ostream& o, const Dealer& x);
std::ostream& operator<<( std::ostream& o, const Shoe& x);
bool operator>( const Player& player, const Dealer& dealer);
bool operator!=( const Player& player, const Dealer& dealer);
}

#endif
