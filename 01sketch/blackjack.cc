
#include <iostream>
#include <cstdlib>
#include <cmath>
using namespace std;

void extract_args( const int argc, const char** argv, int& numRounds);

class Shoe;

class Rules
{
public:
  Rules( const double max_penetration_ratio=0.7)
  {
    m_max_penetration_ratio = max_penetration_ratio;
  }

  bool allow_splitting() const;
  bool allow_doubling_down() const;
  bool allow_doubling_down_after_splitting() const;
  bool say_it_is_time_to_reset_the( const Shoe& shoe) const;
  double max_penetration_ratio() const { return m_max_penetration_ratio;}

private:
  double m_max_penetration_ratio;

};

class Player
{
public:
  Player( const Rules& rules) : m_rules(rules) { m_num_hands = 1;}

  void init( const int round);

  bool has_blackjack();
  bool should_split();
  bool should_double_down();
  bool should_hit();

  int num_hands() { return m_num_hands;}

  void first_hand() { m_hand = 0;}
  bool has_more_hands() { return m_hand<m_num_hands;}
  void next_hand() { m_hand++;}

  void sees( const int dealer_up_card) { m_dealer_up_card = dealer_up_card;}

  void dealt( const int next_card);

  void split();

private:
  int m_hands[4][21];
  int m_hand;
  int m_num_hands;
  int m_num_cards[4];

  int m_hand_value[4];
  int m_num_elevens[4];

  int m_dealer_up_card;

  const Rules& m_rules;

};

class Dealer
{
public:
  Dealer() {}

  void init( const int round);

  bool has_blackjack();
  bool should_hit();

  int up_card();

private:
  int m_hand[21];
  int m_num_cards;

};

class Shoe
{
public:
  Shoe( const int num_decks=6)
  {
    m_num_decks = num_decks;
    m_num_cards_total = 52*m_num_decks;
    reset();
  }

  void reset();

  int next_card();

  void deal_to( Player& player);
  void deal_to( Dealer& dealer);

  int penetration() const { return m_penetration;}
  double penetration_ratio() const;

private:
  int m_cards[10]; // { |10s|, |As|, |2s|, ..., |9s|}
  int m_num_decks;
  int m_num_cards_total;
  int m_penetration; // number of cards dealt

};

class Scribe
{
public:
  Scribe() {}

  void init( const int round) {}

private:

};

class Message
{
public:
  Message( const Rules& rules
         , const Shoe& shoe
         , const Player& player
         , const Dealer& dealer
         , const Scribe& scribe
         )
  : m_rules(rules)
  , m_shoe(shoe)
  , m_player(player)
  , m_dealer(dealer)
  , m_scribe(scribe)
  {}

  void at_beginning_of( int round) const;
  void at_end_of( int round) const;

private:
  const Rules& m_rules;
  const Shoe& m_shoe;
  const Player& m_player;
  const Dealer& m_dealer;
  const Scribe& m_scribe;

};

//##############################################################################

int main( const int argc, const char** argv)
{
  cout << __FILE__ << " " << __LINE__ << " -- " << "Hello." << endl;

  int numRounds = 9;

  extract_args( argc, argv, numRounds);

  Rules rules;

  Shoe shoe;

  Player player(rules);
  Dealer dealer;

  Scribe scribe;

  Message message(rules,shoe,player,dealer,scribe);

  int i;

  int round;

  for( round=1; round<=numRounds; round++)
  {
    message.at_beginning_of( round);

    player.init( round);
    dealer.init( round);
    scribe.init( round);

    shoe.deal_to( player);
    shoe.deal_to( dealer);
    shoe.deal_to( player);
    shoe.deal_to( dealer);

    if( dealer.has_blackjack())
    {
      // TODO
    }
    else
    {
      player.sees( dealer.up_card());

      if( rules.allow_splitting())
      {
        while( player.should_split())
        {
          player.split();
          shoe.deal_to( player);
          shoe.deal_to( player);
        }
      }

      // If splitting is allowed, the player may have up to four hands at this
      // point. Play them one at a time.
      //
      for( player.first_hand(); player.has_more_hands(); player.next_hand())
      {
        if( player.should_double_down())
        {
        }
        else
        {
          if( player.has_blackjack())
          {
          }
          else
          {
            while( player.should_hit())
            {
              shoe.deal_to( player);
            }
          }
        }
      }

      while( dealer.should_hit())
      {
        shoe.deal_to( dealer);
      }

    }

    message.at_end_of( round);

    if( rules.say_it_is_time_to_reset_the( shoe)) { shoe.reset();}

  }

  cout << endl << __FILE__ << " " << __LINE__ << " -- " << "Good Bye." << endl;
  return 0;
}

//##############################################################################

bool Rules::allow_splitting() const
{
  return true;
}

bool Rules::allow_doubling_down() const
{
  return true;
}

bool Rules::allow_doubling_down_after_splitting() const
{
  return true;
}

bool Rules::say_it_is_time_to_reset_the( const Shoe& shoe) const
{
  return shoe.penetration_ratio() >= max_penetration_ratio();
}

//------------------------------------------------------------------------------

bool Player::has_blackjack()
{
  // TODO
  return false;
}

bool Dealer::has_blackjack()
{
  // TODO
  return false;
}

//------------------------------------------------------------------------------

void Dealer::init( const int round)
{
  m_num_cards = 0;
}

int Dealer::up_card()
{
  // TODO
  return 0;
}

bool Dealer::should_hit()
{
  // TODO
  return false;
}

//------------------------------------------------------------------------------

void Player::init( const int round)
{
  m_hand = 0;
  m_num_hands = 1;

  m_num_cards[0] = 0;
  m_num_cards[1] = 0;
  m_num_cards[2] = 0;
  m_num_cards[3] = 0;

  m_hand_value[0] = 0;
  m_hand_value[1] = 0;
  m_hand_value[2] = 0;
  m_hand_value[3] = 0;

  m_num_elevens[0] = 0;
  m_num_elevens[1] = 0;
  m_num_elevens[2] = 0;
  m_num_elevens[3] = 0;

}

bool Player::should_split()
{
  // TODO
  return false;
}

bool Player::should_double_down()
{
  bool val = false;
  if(   ( m_rules.allow_doubling_down())
      ||
        ( num_hands()>1 && m_rules.allow_doubling_down_after_splitting())
    )
  {
  // TODO
  }
  return val;
}

bool Player::should_hit()
{
  int hand_val = m_hand_value[m_hand];
  if( hand_val < 17) { return true;}
  return false;
}

void Player::split()
{
  // TODO
}

void Player::dealt( const int next_card)
{
  m_hands[m_hand][m_num_cards[m_hand]] = next_card;
  m_num_cards[m_hand]++;

  if( next_card==0)
  {
    m_hand_value[m_hand] += 10;
  }
  else if( next_card==1)
  {
    m_hand_value[m_hand] += 11;
    m_num_elevens[m_hand]++;
  }
  else
  {
    m_hand_value[m_hand] += next_card;
  }

  while( m_hand_value[m_hand] > 21 && m_num_elevens[m_hand] > 0)
  {
    m_hand_value[m_hand]-=10;
    m_num_elevens[m_hand]--;
  }
}

//------------------------------------------------------------------------------

void Shoe::reset()
{
  int i;
  for( i=0; i<=10; i++)
  {
    m_cards[i] = m_num_decks*4; // Four of each card per deck, except...
  }
  m_cards[0]*=4; // ...four times as many tens.

  m_penetration = 0;
}

int Shoe::next_card()
{
  int n;

  do
  {
    n = 1+(int)floor(13*((double)rand()/RAND_MAX));
    if( n > 9) { n = 0;}

  } while( !m_cards[n]);

  m_cards[n]--;

  return n;
}

void Shoe::deal_to( Player& player)
{
  player.dealt( next_card());

  m_penetration++;
}

void Shoe::deal_to( Dealer& dealer)
{
  // TODO
  m_penetration++;
}

double Shoe::penetration_ratio() const
{
  return (double)m_penetration / m_num_cards_total;
}

void Message::at_beginning_of( int round) const
{
  cout << endl << "Round " << round << ":" << endl;
}
void Message::at_end_of( int round) const
{
  cout << "  end of round penetration"
       << ": " << m_shoe.penetration()
       << " (" << m_shoe.penetration_ratio() << ")"
       << endl;
}

//##############################################################################

void extract_args( const int argc, const char** argv, int& numRounds)
{
  int n;
  switch( argc)
  {
    case 1: // ./a.exe
    {
      break;
    }

    case 2: // ./a.exe numRounds
    {
      n = atoi(argv[1]);
      if( n) { numRounds = n;}

      break;
    }

    case 3: // ./a.exe numRounds asdf
    {
      break;
    }

    default:
      cout << __FILE__ << " " << __LINE__ << " -- "
           << "WARNING -- Unhandled case: argc==" << argc << endl;
  }
}
