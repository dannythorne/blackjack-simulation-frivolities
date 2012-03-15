
#include <iostream>
#include <cstdlib>
using namespace std;

void extract_args( int argc, char** argv, int& numRounds);

class Rules
{
public:
  Rules() {}

  bool allow_splitting();
  bool allow_doubling_down();

private:

};

class Player
{
public:
  Player() { m_num_hands = 1;}

  bool has_blackjack();
  bool should_split_on( int dealer_up_card);
  bool should_double_down_on( int dealer_up_card);

  int num_hands() { return m_num_hands;}

  void play_hand( int hand) { m_hand = hand;}

  void split();

private:
  int m_hands[4][21];
  int m_hand;
  int m_num_hands;
  int m_num_cards[4];

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
  Shoe() {}

  void deal_to( Player& player);
  void deal_to( Dealer& dealer);

private:

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

  int i;

  int round;

  for( round=1; round<=numRounds; round++)
  {
    cout << "round " << round << endl;

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
      if( rules.allow_splitting())
      {
        while( player.should_split_on( dealer.up_card()))
        {
          player.split();
          shoe.deal_to( player);
          shoe.deal_to( player);
        }
      }

      for( i=1; i<=player.num_hands(); i++)
      {
        player.play_hand( i);

        if( rules.allow_doubling_down()
         && player.should_double_down_on( dealer.up_card()))
        {
        }
        else
        {
        }

      }

    }

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

bool Player::should_split_on( int dealer_up_card)
{
  // TODO
  return false;
}

bool Player::should_double_down_on( int dealer_up_card)
{
  // TODO
  return false;
}

void Player::split()
{
}

//------------------------------------------------------------------------------

void Shoe::deal_to( Player& player)
{
  // TODO
}

void Shoe::deal_to( Dealer& dealer)
{
  // TODO
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
