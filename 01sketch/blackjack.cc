
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath> // Warning: round is redeclared in below scopes.
#include <string>
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
  bool allow_splitting_aces() const;
  bool allow_doubling_down() const;
  bool allow_doubling_down_after_splitting() const;
  bool say_it_is_time_to_reset_the( const Shoe& shoe) const;
  bool say_dealer_hits_soft_17() const;
  double max_penetration_ratio() const { return m_max_penetration_ratio;}

  void display( ostream& o) const;

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

  int num_hands() const { return m_num_hands;}
  int num_cards() const { return m_num_cards[m_hand];}
  int hand() const { return m_hand;}

  void first_hand() { m_hand = 0;}
  bool has_more_hands() { return m_hand<m_num_hands;}
  void next_hand() { m_hand++;}
  bool has_unplayed_hands();

  void sees( const int dealer_up_card) { m_dealer_up_card = dealer_up_card;}

  int value_of_card( const int card) const;

  bool has_aces() const;
  bool has_eights() const;
  bool has_lesser_splittable_pairs() const;

  void dealt( const int next_card);

  void split();
  int num_splits() const { return m_num_splits;}

  bool busts() const;

  void display_hands( ostream& o) const;

private:
  int m_hands[4][21];
  int m_hand;
  int m_num_hands;
  int m_num_cards[4];

  int m_hand_value[4];
  int m_num_elevens[4];

  int m_dealer_up_card;

  int m_num_cards_dealt_since_split[4];
  int m_num_splits;

  int m_num_hands_played;

  const Rules& m_rules;

};

class Dealer
{
public:
  Dealer( const Rules& rules) : m_rules(rules) {}

  void init( const int round);

  bool has_blackjack();
  bool should_hit();

  int up_card();

  int num_cards() const { return m_num_cards;}
  int hand_value() const { return m_hand_value;}

  void dealt( const int next_card);

  bool busts() const;

  void display_hand( ostream& o) const;
  void display_hand_with_only_up_card_showing( ostream& o) const;

private:
  int m_hand[21];
  int m_num_cards;
  int m_hand_value;
  int m_num_elevens;

  const Rules& m_rules;

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

  void init( const int round) { m_round = round;}

  int round() const { return m_round;}

private:

  int m_round;

};

class Message
{
public:
  Message( const Rules& rules
         , const Shoe& shoe
         , const Player& player
         , const Dealer& dealer
         , const Scribe& scribe
         , const bool show=true
         )
  : m_rules(rules)
  , m_shoe(shoe)
  , m_player(player)
  , m_dealer(dealer)
  , m_scribe(scribe)
  {
    m_show = show;
  }

  void before_first_round() const;
  void at_beginning_of_round() const;
  void at_end_of_round() const;
  void after_initial_deal() const;
  void after_dealer_shows_up_card() const;
  void after_player_splits() const;
  void after_player_hits() const;
  void after_player_stands() const;
  void after_dealer_hits() const;
  void after_dealer_stands() const;

  void should_show() { m_show = true;}
  void should_not_show() { m_show = false;}
  bool show() const { return m_show;}

private:
  const Rules& m_rules;
  const Shoe& m_shoe;
  const Player& m_player;
  const Dealer& m_dealer;
  const Scribe& m_scribe;

  bool m_show;

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
  Dealer dealer(rules);

  Scribe scribe;

  Message message(rules,shoe,player,dealer,scribe,/*show*/true);

  message.before_first_round();

  int i;

  int round;

  for( round=1; round<=numRounds; round++)
  {
    scribe.init( round);
    player.init( round);
    dealer.init( round);

    message.at_beginning_of_round();

    shoe.deal_to( player);
    shoe.deal_to( dealer);
    shoe.deal_to( player);
    shoe.deal_to( dealer);

    message.after_initial_deal();

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
          message.after_player_splits();
        }
      }

      while( player.has_unplayed_hands())
      {
        if( player.has_blackjack())
        {
          // TODO:
        }
        else
        {
          if( player.should_double_down())
          {
            // TODO:
          }
          else
          {
            while( player.should_hit())
            {
              shoe.deal_to( player);
              message.after_player_hits();
            }
            message.after_player_stands();
          }
        }
      }

      message.after_dealer_shows_up_card();

      while( dealer.should_hit())
      {
        shoe.deal_to( dealer);
        message.after_dealer_hits();
      }
      message.after_dealer_stands();

    }

    message.at_end_of_round();

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

bool Rules::allow_splitting_aces() const
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

bool Rules::say_dealer_hits_soft_17() const
{
  return false;
}

void Rules::display( ostream& o) const
{
  o << "  allow splitting ------------------ : "
    << ( (allow_splitting())?("yes"):("no"));
  o << endl;

  o << "  allow splitting aces ------------- : "
    << ( (allow_splitting_aces())?("yes"):("no"));
  o << endl;

  o << "  allow doubling down -------------- : "
    << ( (allow_doubling_down())?("yes"):("no"));
  o << endl;

  o << "  allow doubling down after splitting: "
    << ( (allow_doubling_down_after_splitting())?("yes"):("no"));
  o << endl;

  o << "  dealer hits soft 17 -------------- : "
    << ( (say_dealer_hits_soft_17())?("yes"):("no"));
  o << endl;
}

//##############################################################################

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

//##############################################################################

void Dealer::init( const int round)
{
  m_num_cards = 0;
  m_hand_value = 0;
  m_num_elevens = 0;
}

int Dealer::up_card()
{
  // TODO
  return 0;
}

bool Dealer::should_hit()
{
  int hand_val = m_hand_value;

  if( m_hand_value < 17)
  {
    return true;
  }
  else if( m_hand_value==17 && m_rules.say_dealer_hits_soft_17())
  {
    return true;
  }

  return false;
}

void Dealer::dealt( const int next_card)
{
  m_hand[m_num_cards] = next_card;
  m_num_cards++;

  if( next_card==0)
  {
    m_hand_value += 10;
  }
  else if( next_card==1)
  {
    m_hand_value += 11;
    m_num_elevens++;
  }
  else
  {
    m_hand_value += next_card;
  }

  while( m_hand_value > 21 && m_num_elevens > 0)
  {
    m_hand_value-=10;
    m_num_elevens--;
  }
}

bool Dealer::busts() const
{
  return m_hand_value > 21;
}

void Dealer::display_hand_with_only_up_card_showing( ostream& o) const
{
  if( m_num_cards > 2)
  {
    cerr << __FILE__ << " " << __LINE__ << " -- "
         << "ERROR: dealer should only have two cards when hole card is hidden."
         << endl;
    exit(1);
  }
  int i;
  o << " ??";
  o << " [";
  o << m_hand[0];
  o << "#";
  o << "];";
}

void Dealer::display_hand( ostream& o) const
{
  int i;
  o << " " << setw(2) << m_hand_value;
  o << " [";
  for( i=0; i<num_cards(); i++)
  {
    o << m_hand[i];
  }
  o << "];";
}

//##############################################################################

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

  m_num_splits = 0;
  m_num_cards_dealt_since_split[0] = 0;
  m_num_cards_dealt_since_split[1] = 0;
  m_num_cards_dealt_since_split[2] = 0;
  m_num_cards_dealt_since_split[3] = 0;

  m_num_hands_played = 0;

}

bool Player::has_eights() const
{
  if( num_cards()>2) { return false;}
  return m_hands[m_hand][0] == 8 && m_hands[m_hand][1] == 8;
}

bool Player::has_aces() const
{
  if( num_cards()>2) { return false;}
  return m_hands[m_hand][0] == 1 && m_hands[m_hand][1] == 1;
}

bool Player::has_lesser_splittable_pairs() const
{
  if( num_cards()>2) { return false;}
  return ( m_hands[m_hand][0] == 2 && m_hands[m_hand][1] == 2)
      || ( m_hands[m_hand][0] == 3 && m_hands[m_hand][1] == 3)
      || ( m_hands[m_hand][0] == 6 && m_hands[m_hand][1] == 6)
      || ( m_hands[m_hand][0] == 7 && m_hands[m_hand][1] == 7)
      || ( m_hands[m_hand][0] == 9 && m_hands[m_hand][1] == 9);
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

bool Player::has_unplayed_hands()
{
  if( m_num_hands_played < m_num_hands)
  {
    m_hand = m_num_hands_played;
    m_num_hands_played++;
    return true;
  }
  return false;
}

int Player::value_of_card( const int card) const
{
  // WARNING: Returns value of 11 for all aces. Calling routine must determine
  // whether ace should count as 1 instead.

  int card_val;

  if( card==0)
  {
    card_val = 10;
  }
  else if( card==1)
  {
    card_val = 11;
  }
  else
  {
    card_val = card;
  }

  return card_val;
}

bool Player::should_split()
{
  if( m_num_splits<3)
  {
    if(/*test splitting mechanism*/false)
    {
      return true;
    }
    for( m_hand=0; m_hand<num_hands(); m_hand++)
    {
      if( has_eights())
      {
        return true;
      }
      else if( m_rules.allow_splitting_aces() && has_aces())
      {
        return true;
      }
      else if( ( m_dealer_up_card>=2 || m_dealer_up_card<=6)
             &&( has_lesser_splittable_pairs()) )
      {
        return true;
      }
    }
    m_hand = 0;
  }
  return false;
}

void Player::split()
{
  m_num_cards_dealt_since_split[m_num_splits] = 0;
  m_num_splits++;

  switch( m_num_splits)
  {
    case 1:
    {
      if( m_num_cards[0]!=2)
      {
        cerr << __FILE__ << " " << __LINE__ << " -- "
             << "ERROR -- Attempt to split a hand of "
             << m_num_cards[0] << " cards."
             << endl;
        exit(1);
      }

#if 0
      m_hands[1][0] = m_hands[0][1];

      m_num_cards[0]=1;
      m_hand_value[0] = value_of_card( m_hands[0][0]);

      m_num_cards[1]=1;
      m_hand_value[1] = value_of_card( m_hands[1][0]);

      m_num_hands++;
#else
      int new_hand = m_num_splits;

      m_hands[new_hand][0] = m_hands[m_hand][1];

      m_num_cards[m_hand]=1;
      m_hand_value[m_hand] = value_of_card( m_hands[m_hand][0]);

      m_num_cards[new_hand]=1;
      m_hand_value[new_hand] = value_of_card( m_hands[new_hand][0]);

      m_num_hands++;
#endif

      if( m_num_hands!=2)
      {
        cerr << __FILE__ << " " << __LINE__ << " -- "
             << "BOOM!"
             << endl;
        exit(1);
      }
      break;
    }
    case 2:
    {
      if( m_num_cards[m_hand]!=2)
      {
        cerr << __FILE__ << " " << __LINE__ << " -- "
             << "ERROR -- Attempt to split a hand of "
             << m_num_cards[0] << " cards."
             << endl;
        exit(1);
      }

      int new_hand = m_num_splits;

      m_hands[new_hand][0] = m_hands[m_hand][1];

      m_num_cards[m_hand]=1;
      m_hand_value[m_hand] = value_of_card( m_hands[m_hand][0]);

      m_num_cards[new_hand]=1;
      m_hand_value[new_hand] = value_of_card( m_hands[new_hand][0]);

      m_num_hands++;

      if( m_num_hands!=3)
      {
        cerr << __FILE__ << " " << __LINE__ << " -- "
             << "BOOM!"
             << endl;
        exit(1);
      }

      break;
    }
    case 3:
    {
      if( m_num_cards[m_hand]!=2)
      {
        cerr << __FILE__ << " " << __LINE__ << " -- "
             << "ERROR -- Attempt to split a hand of "
             << m_num_cards[0] << " cards."
             << endl;
        exit(1);
      }

      int new_hand = m_num_splits;

      m_hands[new_hand][0] = m_hands[m_hand][1];

      m_num_cards[m_hand]=1;
      m_hand_value[m_hand] = value_of_card( m_hands[m_hand][0]);

      m_num_cards[new_hand]=1;
      m_hand_value[new_hand] = value_of_card( m_hands[new_hand][0]);

      m_num_hands++;

      if( m_num_hands!=4)
      {
        cerr << __FILE__ << " " << __LINE__ << " -- "
             << "BOOM!"
             << endl;
        exit(1);
      }

      break;
    }
    default:
    {
      cerr << __FILE__ << " " << __LINE__ << " -- "
           << "ERROR -- Unhandled case: m_num_splits = "
           << m_num_splits << endl;
      exit(1);
      break;
    }
  }
}

void Player::dealt( const int next_card)
{

  if(   m_num_splits > 0
     && m_num_cards_dealt_since_split[m_num_splits-1]<2)
  {
    switch( m_num_splits)
    {
      case 1:
      {
        if( m_num_cards_dealt_since_split[0]==0)
        {
          m_hands[0][m_num_cards[0]] = next_card;
          m_num_cards[0]++;
          m_num_cards_dealt_since_split[0]++;
          m_hand_value[0] += value_of_card( m_hands[0][1]);
        }
        else if( m_num_cards_dealt_since_split[0]==1)
        {
          m_hands[1][m_num_cards[1]] = next_card;
          m_num_cards[1]++;
          m_num_cards_dealt_since_split[0]++;
          m_hand_value[1] += value_of_card( m_hands[1][1]);
        }
        else
        {
          cerr << __FILE__ << " " << __LINE__ << " -- "
               << "ERROR -- Unhandled case: "
               << "m_num_cards_dealt_since_split[0] = "
               << m_num_cards_dealt_since_split[0]
               << endl;
          exit(1);
        }
        break;
      }
      case 2:
      {
        int new_hand = m_num_splits;
        if( m_num_cards_dealt_since_split[m_num_splits-1]==0)
        {
          m_hands[m_hand][m_num_cards[m_hand]] = next_card;
          m_num_cards[m_hand]++;
          m_num_cards_dealt_since_split[m_num_splits-1]++;
          m_hand_value[m_hand] += value_of_card( m_hands[m_hand][1]);
        }
        else if( m_num_cards_dealt_since_split[m_num_splits-1]==1)
        {
          m_hands[new_hand][m_num_cards[new_hand]] = next_card;
          m_num_cards[new_hand]++;
          m_num_cards_dealt_since_split[m_num_splits-1]++;
          m_hand_value[new_hand] += value_of_card( m_hands[new_hand][1]);
        }
        else
        {
          cerr << __FILE__ << " " << __LINE__ << " -- "
               << "ERROR -- Unhandled case: "
               << "m_num_cards_dealt_since_split["
               << m_num_splits-1
               << "] = "
               << m_num_cards_dealt_since_split[m_num_splits-1]
               << endl;
          exit(1);
        }
        break;
      }
      case 3:
      {
        int new_hand = m_num_splits;
        if( m_num_cards_dealt_since_split[m_num_splits-1]==0)
        {
          m_hands[m_hand][m_num_cards[m_hand]] = next_card;
          m_num_cards[m_hand]++;
          m_num_cards_dealt_since_split[m_num_splits-1]++;
          m_hand_value[m_hand] += value_of_card( m_hands[m_hand][1]);
        }
        else if( m_num_cards_dealt_since_split[m_num_splits-1]==1)
        {
          m_hands[new_hand][m_num_cards[new_hand]] = next_card;
          m_num_cards[new_hand]++;
          m_num_cards_dealt_since_split[m_num_splits-1]++;
          m_hand_value[new_hand] += value_of_card( m_hands[new_hand][1]);
        }
        else
        {
          cerr << __FILE__ << " " << __LINE__ << " -- "
               << "ERROR -- Unhandled case: "
               << "m_num_cards_dealt_since_split["
               << m_num_splits-1
               << "] = "
               << m_num_cards_dealt_since_split[m_num_splits-1]
               << endl;
          exit(1);
        }
        break;
      }
      default:
      {
        cerr << __FILE__ << " " << __LINE__ << " -- "
             << "ERROR -- Unhandled case: m_num_splits = "
             << m_num_splits << endl;
        exit(1);
        break;
      }
    }
  }
  else
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
}

bool Player::busts() const
{
  return m_hand_value[m_hand]>21;
}

void Player::display_hands( ostream& o) const
{
  int i, j;

  // TODO: Display cards like this?
  //
  // +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+
  // |   | |   | |   | |   | |   | |   | |   | |   | |   | |   |
  // | 0 | | 1 | | 2 | | 3 | | 4 | | 5 | | 6 | | 7 | | 8 | | 9 |
  // |   | |   | |   | |   | |   | |   | |   | |   | |   | |   |
  // +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+
  //

  for( j=0; j<num_hands(); j++)
  {
    o << " " << setw(2) << m_hand_value[j];
    o << " ";
    o << "[";
    for( i=0; i<m_num_cards[j]; i++)
    {
      o << m_hands[j][i];
    }
    o << "]";
    o << ";";
  }
}

//##############################################################################

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
  dealer.dealt( next_card());

  m_penetration++;
}

double Shoe::penetration_ratio() const
{
  return (double)m_penetration / m_num_cards_total;
}

//##############################################################################

void Message::before_first_round() const
{
  if( m_show)
  {
    cout << endl;
    cout << "Rules:" << endl;
    m_rules.display(cout);

    cout << endl;
    cout << "Notation: ";
    cout << "{ 0, 1, 2, ..., 9} --> { {10,J,K,Q}, A, 2, ..., 9}" << endl;

    cout << string(72,'=') << endl;
  }
}
void Message::at_beginning_of_round() const
{
  if( m_show)
  {
    cout << endl << "Round " << m_scribe.round() << ":" << endl;
  }
}

void Message::after_initial_deal() const
{
  if( m_show)
  {
    cout << endl;
    cout << "  dealer: ";
    m_dealer.display_hand_with_only_up_card_showing(cout);
    cout << endl;

    cout << "  player: ";
    m_player.display_hands(cout);
    cout << endl;
  }
}

void Message::after_dealer_shows_up_card() const
{
  if( m_show)
  {
    cout << endl;
    cout << "  dealer: ";
    m_dealer.display_hand(cout);
    cout << endl;
  }
}

void Message::after_player_splits() const
{
  if( m_show)
  {
    cout << endl;
    if( m_player.num_splits()>1)
    {
      cout << "  player splits hand "
           << m_player.hand()+1;
    }
    else
    {
      cout << "  player splits";
    }
    cout << " with: ";
    m_player.display_hands(cout);
    cout << endl;
  }
}

void Message::after_player_hits() const
{
  if( m_show)
  {
    cout << endl;
    cout << "  player hits";
    if( m_player.num_hands()>1)
    {
      cout << " hand " << m_player.hand()+1;
    }
    cout << ": ";
    m_player.display_hands(cout);
    cout << endl;
  }
}

void Message::after_player_stands() const
{
  if( m_show)
  {
    cout << endl;
    if( m_player.busts())
    {
      cout << "  player busts";
    }
    else
    {
      cout << "  player stands";
    }
    if( m_player.num_hands()>1)
    {
      cout << " on hand " << m_player.hand()+1;
    }
    cout << " with: ";
    m_player.display_hands(cout);
    cout << endl;
  }
}

void Message::after_dealer_hits() const
{
  if( m_show)
  {
    cout << endl;
    cout << "  *** dealer hits ***" << endl;
    cout << endl;
    cout << "  dealer: ";
    m_dealer.display_hand(cout);
    cout << endl;
  }
}

void Message::after_dealer_stands() const
{
  if( m_show)
  {
    cout << endl;
    if( m_dealer.busts())
    {
      cout << "  *** dealer busts ***" << endl;
    }
    else
    {
      cout << "  *** dealer stands ***" << endl;
    }
  }
}

void Message::at_end_of_round() const
{
  if( m_show)
  {
    cout << endl;
    cout << "  penetration"
         << ": " << m_shoe.penetration()
         << " (" << m_shoe.penetration_ratio() << ")"
         << endl;
    cout << string(72,'-') << endl;
  }
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

    case 3: // ./a.exe numRounds ----
    {
      break;
    }

    default:
      cout << __FILE__ << " " << __LINE__ << " -- "
           << "WARNING -- Unhandled case: argc==" << argc << endl;
  }
}
