
#include <iostream>
#include <cstdlib>
using namespace std;

void extract_args( int argc, char** argv, int& numRounds);

class Shoe;

class Rules
{
public:
  Rules( double max_penetration_ratio=0.7) { m_max_penetration_ratio = max_penetration_ratio;}

  bool allow_splitting();
  bool allow_doubling_down();
  bool allow_doubling_down_after_splitting();
  bool say_it_is_time_to_reset_the( Shoe& shoe);
  double max_penetration_ratio() { return m_max_penetration_ratio;}

private:
  double m_max_penetration_ratio;

};

class Player
{
public:
  Player() { m_num_hands = 1;}

  bool has_blackjack();
  bool should_split();
  bool should_double_down();
  bool should_hit();

  int num_hands() { return m_num_hands;}

  void first_hand() { m_hand = 1;}
  bool has_more_hands() { return m_hand<m_num_hands;}
  void next_hand() { m_hand++;}

  void sees( int dealer_up_card) { m_dealer_up_card = dealer_up_card;}

  void split();

private:
  int m_hands[4][21];
  int m_hand;
  int m_num_hands;
  int m_num_cards[4];
  int m_dealer_up_card;

};

class Dealer
{
public:
  Dealer() {}
  bool has_blackjack();
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

  void deal_to( Player& player);
  void deal_to( Dealer& dealer);

  int penetration() { return m_penetration;}
  double penetration_ratio();

private:
  int m_cards[10]; // { |10s|, |As|, |2s|, ..., |9s|}
  int m_num_decks;
  int m_num_cards_total;
  int m_penetration;

};

class Message
{
public:
  Message( Rules& rules
         , Shoe& shoe
         , Player& player
         , Dealer& dealer)
  : m_rules(rules)
  , m_shoe(shoe)
  , m_player(player)
  , m_dealer(dealer)
  {}

  void at_beginning_of( int round);
  void at_end_of( int round);

private:
  Rules& m_rules;
  Shoe& m_shoe;
  Player& m_player;
  Dealer& m_dealer;

};

//##############################################################################

int main( int argc, char** argv)
{
  cout << __FILE__ << " " << __LINE__ << " -- " << "Hello." << endl;

  int numRounds = 9;

  extract_args( argc, argv, numRounds);

  Rules rules;

  Shoe shoe;

  Player player;
  Dealer dealer;

  Message message(rules,shoe,player,dealer);

  int i;

  int round;

  for( round=1; round<=numRounds; round++)
  {
    message.at_beginning_of( round);

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
        if(  (   (   rules.allow_doubling_down())
              ||
                 (   player.num_hands()>1
                  && rules.allow_doubling_down_after_splitting())
             )
           && player.should_double_down())
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
            }
          }
        }

      }

    }

    message.at_end_of( round);

    if( rules.say_it_is_time_to_reset_the( shoe)) { shoe.reset();}

  }

  cout << __FILE__ << " " << __LINE__ << " -- " << "Good Bye." << endl;
  return 0;
}

//##############################################################################

bool Rules::allow_splitting()
{
  return true;
}

bool Rules::allow_doubling_down()
{
  return true;
}

bool Rules::allow_doubling_down_after_splitting()
{
  return true;
}

bool Rules::say_it_is_time_to_reset_the( Shoe& shoe)
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

int Dealer::up_card()
{
  // TODO
  return 0;
}

//------------------------------------------------------------------------------

bool Player::should_split()
{
  // TODO
  return false;
}

bool Player::should_double_down()
{
  // TODO
  return false;
}

bool Player::should_hit()
{
  // TODO
  return false;
}

void Player::split()
{
  // TODO
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

void Shoe::deal_to( Player& player)
{
  // TODO
  m_penetration++;
}

void Shoe::deal_to( Dealer& dealer)
{
  // TODO
  m_penetration++;
}

double Shoe::penetration_ratio()
{
  return (double)m_penetration / m_num_cards_total;
}

void Message::at_beginning_of( int round)
{
  cout << endl << "Round " << round << ":" << endl;
}
void Message::at_end_of( int round)
{
  cout << "  end of round penetration"
       << ": " << m_shoe.penetration()
       << " (" << m_shoe.penetration_ratio() << ")"
       << endl;
}

//##############################################################################

void extract_args( int argc, char** argv, int& numRounds)
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
