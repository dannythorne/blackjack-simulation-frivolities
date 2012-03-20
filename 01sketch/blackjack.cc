
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath> // Warning: round is redeclared in below scopes.
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
using namespace std;

namespace dthorne0_blackjack {
//##############################################################################

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
  Player( const Rules& rules
        , const float initial_bankroll=20.0
        , const float base_wager=1.0
        , const float martingale_factor=2.0
        )
  : m_rules(rules)
  {
    m_num_hands = 1;

    m_initial_bankroll = initial_bankroll;
    m_base_wager = base_wager;

    m_martingale_factor = martingale_factor; // TODO: <-- Martingale betting
  }

  void prepares_for_new_game();
  void prepares_for_new_round();

  void places_bet();

  bool should_hit() const;

  bool should_split() const;

  bool can_double_down() const;
  bool should_double_down() const;
  void doubles_down();
  bool has_hard_nine() const;
  bool has_soft_sixteen_through_eighteen() const;

  int num_hands() const { return m_num_hands;}
  int num_cards() const { return m_num_cards[m_hand];}
  int hand() const { return m_hand;}
  int cur_hand() const { return m_hand;}
  int hand_value() const { return m_hand_value[m_hand];}

  void first_hand() const { m_hand = 0;}
  bool has_more_hands() const { return m_hand<m_num_hands;}
  void next_hand() const { m_hand++;}
  bool has_unplayed_hands() const;

  void sees( const int dealer_up_card) { m_dealer_up_card = dealer_up_card;}

  int value_of_card( const int card) const;

  bool has_blackjack() const;
  bool has_aces() const;
  bool has_eights() const;
  bool has_lesser_splittable_pairs() const;

  void won();
  void lost();
  void got_blackjack();

  void is_dealt( const int next_card);

  void split();
  int num_splits() const { return m_num_splits;}

  bool busted() const;
  bool busted_all_hands() const;

  void display_hands( ostream& o) const;
  void display_hand( ostream& o) const;

  float wager() const;
  float base_wager() const { return m_base_wager;}
  float initial_bankroll() const { return m_initial_bankroll;}
  float winnings() const { return m_bankroll;}

  bool bankroll_is_up() const { return m_bankroll > m_initial_bankroll;}
  bool bankroll_is_down() const { return m_bankroll < m_initial_bankroll;}

private:

  const Rules& m_rules;

  int m_hands[4][21];
  mutable int m_hand;
  int m_num_hands;
  int m_num_cards[4];

  int m_hand_value[4];
  int m_num_elevens[4];

  int m_dealer_up_card;

  int m_num_cards_dealt_since_split[4];
  int m_num_splits;

  mutable int m_num_hands_played;

  float m_wager;
  float m_base_wager;
  float m_initial_bankroll;
  float m_bankroll;

  float m_martingale_factor;
  float m_martingale_wager;
  float m_martingale_bankroll;

};

class Dealer
{
public:
  Dealer( const Rules& rules) : m_rules(rules) {}

  void prepares_for_new_game();
  void prepares_for_new_round();

  bool has_blackjack() const;
  bool should_hit() const;

  int up_card();

  int num_cards() const { return m_num_cards;}
  int hand_value() const { return m_hand_value;}

  void is_dealt( const int next_card);

  bool busted() const;

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

  void deals_to( Player& player);
  void deals_to( Dealer& dealer);

  int penetration() const { return m_penetration;}
  double penetration_ratio() const;

  void display( ostream& o) const;

private:
  int m_cards[10]; // { |10s|, |As|, |2s|, ..., |9s|}
  int m_num_decks;
  int m_num_cards_total;
  int m_penetration; // number of cards dealt

};

class Scribe
{
public:
  Scribe( const Rules& rules
        , const Shoe& shoe
        , const Player& player
        , const Dealer& dealer
        , const int num_rounds_to_play=9
        )
  : m_rules(rules)
  , m_shoe(shoe)
  , m_player(player)
  , m_dealer(dealer)
  {
    m_num_rounds_to_play = num_rounds_to_play;
    m_num_rounds_this_game = 0;
    m_num_hands_this_game = 0;
    m_max_hands_in_a_single_game = 0;
    m_max_rounds_in_a_single_game = 0;
    m_num_rounds_total = 0;
    m_num_hands_total = 0;
    m_num_wins_total = 0;
    m_num_losses_total = 0;
    m_num_pushes_total = 0;
    m_num_blackjacks_total = 0;

    m_num_games = -1;

    m_win_counts.reserve( 1*m_num_rounds_to_play);
    m_bankroll.reserve( 1*m_num_rounds_to_play);
    m_bankroll_up_counts.reserve( 1*m_num_rounds_to_play);
    m_bankroll_down_counts.reserve( 1*m_num_rounds_to_play);
    m_hand_counts.reserve( 1*m_num_rounds_to_play);
    m_round_counts.reserve( 1*m_num_rounds_to_play);

    m_win_counts.push_back(0);
    m_bankroll.push_back(m_player.initial_bankroll());
    m_bankroll_up_counts.push_back(0);
    m_bankroll_down_counts.push_back(0);
    m_hand_counts.push_back(0);
    m_round_counts.push_back(0);

    m_start_a_new_game = true;
  }

  void prepares_for_new_game();
  void prepares_for_new_round();

  int cur_game() const { return m_num_games+1;}
  int num_games() const { return m_num_games;}

  int cur_round() const { return m_num_rounds_this_game+1;}
  int num_rounds() const { return m_num_rounds_this_game;}

  int cur_hand() const { return m_num_hands_this_game+1;}
  int num_hands() const { return m_num_hands_this_game;}

  bool says_to_keep_playing() const;
  bool says_to_start_a_new_game() const;

  void prepares_to_record_results();
  void considers_the_results();
  void summarizes_results_to_stdout();
  void writes_results_to_matlab_scripts();

  bool sees_that_player_has_blackjack() const;

  void records_that_the_player_busted();
  void records_that_the_player_won();
  void records_that_the_player_lost();
  void records_a_push();
  void records_that_the_player_got_blackjack();

private:

  const Rules& m_rules;
  const Shoe& m_shoe;
  const Player& m_player;
  const Dealer& m_dealer;

  int m_num_rounds_this_game;
  int m_max_rounds_in_a_single_game;
  int m_num_rounds_total;
  int m_num_hands_total;
  int m_num_wins_total;
  int m_num_losses_total;
  int m_num_pushes_total;
  int m_num_blackjacks_total;

  int m_num_rounds_to_play;

  int m_num_games;

  int m_num_hands_this_game;
  int m_max_hands_in_a_single_game;

  vector<int> m_win_counts;
  vector<float> m_bankroll;
  vector<int> m_bankroll_up_counts;
  vector<int> m_bankroll_down_counts;
  vector<int> m_hand_counts;
  vector<int> m_round_counts;

  bool player_lost_all_their_money() const;
  void make_more_space_if_necessary();

  bool m_start_a_new_game;

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
  void at_beginning_of_game() const;
  void at_end_of_round() const;
  void after_initial_deal() const;
  void after_dealer_shows_hole_card() const;
  void after_player_splits() const;
  void after_last_split() const;
  void after_player_hits() const;
  void after_player_stands() const;
  void after_dealer_hits() const;
  void after_dealer_stands() const;
  void before_shoe_reset() const;
  void after_shoe_reset() const;
  void when_player_gets_blackjack() const;
  void when_dealer_gets_blackjack() const;

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

}

//##############################################################################

int main( const int argc, const char** argv)
{
  using namespace dthorne0_blackjack;

  cout << __FILE__ << " " << __LINE__ << " -- " << "Hello." << endl;
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
    seed = 1;

  }

  srand( seed);


  int num_rounds_to_play = 9;

  extract_args( argc, argv, num_rounds_to_play);

  Rules rules;

  Shoe shoe;

  Player player(rules);
  Dealer dealer(rules);

  Scribe scribe( rules,shoe,player,dealer, num_rounds_to_play);

  Message message( rules,shoe,player,dealer,scribe, /*show*/false);

  message.before_first_round();

  while( scribe.says_to_keep_playing())
  {
    if( scribe.says_to_start_a_new_game())
    {
      player.prepares_for_new_game();
      dealer.prepares_for_new_game();
      scribe.prepares_for_new_game();
      message.at_beginning_of_game();
    }
    message.at_beginning_of_round();

    player.prepares_for_new_round();
    dealer.prepares_for_new_round();
    scribe.prepares_for_new_round();

    player.places_bet();

    shoe.deals_to( player);
    shoe.deals_to( dealer);
    shoe.deals_to( player);
    shoe.deals_to( dealer);

    message.after_initial_deal();

    if( dealer.has_blackjack())
    {
      message.when_dealer_gets_blackjack();
      if( player.has_blackjack())
      {
        message.when_player_gets_blackjack();
      }
    }
    else
    {
      player.sees( dealer.up_card());

      if( rules.allow_splitting())
      {
        while( player.should_split())
        {
          player.split();
          shoe.deals_to( player);
          shoe.deals_to( player);
          message.after_player_splits();
        }
        message.after_last_split();
      }

      while( player.has_unplayed_hands())
      {
        if( player.has_blackjack())
        {
          message.when_player_gets_blackjack();
        }
        else
        {
          if( player.can_double_down() && player.should_double_down())
          {
            player.doubles_down();
          }
          else
          {
            while( player.should_hit())
            {
              shoe.deals_to( player);
              message.after_player_hits();
            }
            message.after_player_stands();
          }
        }
      }

      if( !player.has_blackjack() && !player.busted_all_hands())
      {
        message.after_dealer_shows_hole_card();

        while( dealer.should_hit())
        {
          shoe.deals_to( dealer);
          message.after_dealer_hits();
        }
        message.after_dealer_stands();
      }

    }

    scribe.prepares_to_record_results();

    for( player.first_hand(); player.has_more_hands(); player.next_hand())
    {
      if( player.busted())
      {
        player.lost();
        scribe.records_that_the_player_busted();
      }
      else if( dealer.busted())
      {
        player.won();
        scribe.records_that_the_player_won();
      }
      else
      {
        if( player.hand_value() < dealer.hand_value())
        {
          player.lost();
          scribe.records_that_the_player_lost();
        }
        else if( player.hand_value() > dealer.hand_value())
        {
          if( scribe.sees_that_player_has_blackjack())
          {
            player.got_blackjack();
            scribe.records_that_the_player_got_blackjack();
          }
          else
          {
            player.won();
            scribe.records_that_the_player_won();
          }
        }
        else
        {
          scribe.records_a_push();
        }
      }
    }

    scribe.considers_the_results();

    message.at_end_of_round();

    if( rules.say_it_is_time_to_reset_the( shoe)
      ||scribe.says_to_start_a_new_game()
      )
    {
      message.before_shoe_reset();
      shoe.reset();
      message.after_shoe_reset();
    }

  }

  scribe.summarizes_results_to_stdout();
  scribe.writes_results_to_matlab_scripts();

  cout << endl << __FILE__ << " " << __LINE__ << " -- " << "Good Bye." << endl;
  return 0;
}

//##############################################################################

namespace dthorne0_blackjack { // args
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
}

namespace dthorne0_blackjack { // Rules
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

}

namespace dthorne0_blackjack { // Dealer
//##############################################################################

bool Dealer::has_blackjack() const
{
  if( m_num_cards==2 && hand_value() == 21)
  {
    return true;
  }
  return false;
}

void Dealer::prepares_for_new_game()
{
  // TODO:
}

void Dealer::prepares_for_new_round()
{
  m_num_cards = 0;
  m_hand_value = 0;
  m_num_elevens = 0;
}

int Dealer::up_card()
{
  return m_hand[0];
}

bool Dealer::should_hit() const
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

void Dealer::is_dealt( const int next_card)
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

bool Dealer::busted() const
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

}

namespace dthorne0_blackjack { // Player
//##############################################################################

bool Player::has_blackjack() const
{
  if( m_num_hands==1 && m_num_cards[0]==2)
  {
    if( hand_value() == 21)
    {
      return true;
    }
  }
  return false;
}

void Player::prepares_for_new_game()
{
  m_bankroll = initial_bankroll();
}

void Player::prepares_for_new_round()
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

void Player::places_bet()
{
  // TODO:
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

void Player::won()
{
  m_bankroll += wager();
}

void Player::lost()
{
  m_bankroll -= wager();
}

void Player::got_blackjack()
{
  m_bankroll += 1.5*wager();
}

bool Player::can_double_down() const
{
  bool val = false;
  if(   ( m_rules.allow_doubling_down())
      ||
        ( num_hands()>1 && m_rules.allow_doubling_down_after_splitting())
    )
  {
    if( m_bankroll > 2*m_wager)
    {
      val = true;
    }
  }
  return val;
}
bool Player::should_double_down() const
{
  bool val = false;
  if(  m_dealer_up_card > 1 && m_dealer_up_card < 7
    && ( has_hard_nine() || has_soft_sixteen_through_eighteen()))
  {
    val = true;
  }
  return val;
}

bool Player::has_hard_nine() const
{
  return hand_value()==9;
}

bool Player::has_soft_sixteen_through_eighteen() const
{
  if( m_num_elevens == 0)
  {
    if( hand_value()>=16 && hand_value()<=18)
    {
      return true;
    }
  }
  return false;
}

void Player::doubles_down()
{
  m_wager*=2;
}

bool Player::should_hit() const
{
  int hand_val = m_hand_value[m_hand];
  if( hand_val < 17) { return true;}
  return false;
}

bool Player::has_unplayed_hands() const
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

bool Player::should_split() const
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

void Player::is_dealt( const int next_card)
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

bool Player::busted() const
{
  return m_hand_value[m_hand]>21;
}

bool Player::busted_all_hands() const
{
  int i;
  for( i=0; i<m_num_hands; i++)
  {
    if( m_hand_value[i] <= 21)
    {
      return false;
    }
  }
  return true;
}

void Player::display_hands( ostream& o) const
{
  int i, j;

  // Display cards like this?
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

void Player::display_hand( ostream& o) const
{
  int i;
  o << " " << setw(2) << m_hand_value[m_hand];
  o << " ";
  o << "[";
  for( i=0; i<m_num_cards[m_hand]; i++)
  {
    o << m_hands[m_hand][i];
  }
  o << "]";
  o << ";";
}

float Player::wager() const
{
  if( base_wager() > winnings())
  {
    return winnings();
  }
  else
  {
    return base_wager();
  }
}

}

namespace dthorne0_blackjack { // Shoe
//##############################################################################

void Shoe::reset()
{
  int i;
  for( i=0; i<10; i++)
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

void Shoe::deals_to( Player& player)
{
  player.is_dealt( next_card());

  m_penetration++;
}

void Shoe::deals_to( Dealer& dealer)
{
  dealer.is_dealt( next_card());

  m_penetration++;
}

double Shoe::penetration_ratio() const
{
  return (double)m_penetration / m_num_cards_total;
}

void Shoe::display( ostream& o) const
{
  int i;
  cout << "  shoe:";
  for( i=0; i<10; i++)
  {
    o << " " << m_cards[i];
  }
  o << " -- penetration " << penetration()
    << " ( ratio "
    << penetration_ratio()
    << ")";
  o << endl;
}

}

namespace dthorne0_blackjack { // Message
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

void Message::at_beginning_of_game() const
{
  if( m_show)
  {
    cout << endl << " *** Starting game " << m_scribe.cur_game() << " ***"
         << endl;
  }
}

void Message::at_beginning_of_round() const
{
  if( m_show)
  {
    cout << endl;
    cout << "Round " << m_scribe.cur_round()
         << ", " << "Game " << m_scribe.cur_game()
         << ":" << endl;

    cout << endl;
    cout << "  Wager: $" << m_player.wager() << endl;
  }
}

void Message::after_initial_deal() const
{
  if( m_show)
  {
    cout << endl;
    cout << "    dealer: ";
    m_dealer.display_hand_with_only_up_card_showing(cout);
    cout << endl;

    cout << "    player: ";
    m_player.display_hands(cout);
    cout << endl;
    cout << endl;
  }
}

void Message::after_dealer_shows_hole_card() const
{
  if( m_show)
  {
    cout << "    dealer       : ";
    m_dealer.display_hand(cout);
    cout << endl;
  }
}

void Message::after_player_splits() const
{
  if( m_show)
  {
    if( m_player.num_splits()>1)
    {
      cout << "    player splits hand "
           << m_player.hand()+1;
    }
    else
    {
      cout << "    player splits       ";
    }
    cout << ": ";
    m_player.display_hands(cout);
    cout << endl;
  }
}

void Message::after_last_split() const
{
  if( m_show)
  {
    cout << endl;
  }
}

void Message::after_player_hits() const
{
  if( m_show)
  {
    cout << "    player *hits*";
    if( m_player.num_hands()>1)
    {
      cout << " hand " << m_player.hand()+1;
    }
    cout << ": ";
    m_player.display_hand(cout);
    if( m_player.busted())
    {
      cout << " *bust*";
      cout << endl;
    }
    cout << endl;
  }
}

void Message::after_player_stands() const
{
  if( m_show)
  {
    if( !m_player.busted())
    {
      cout << "    player stands";
      if( m_player.num_hands()>1)
      {
        cout << " hand " << m_player.hand()+1;
      }
      cout << ": ";
      m_player.display_hand(cout);
      cout << endl;
      cout << endl;
    }
  }
}

void Message::after_dealer_hits() const
{
  if( m_show)
  {
    cout << "    dealer *hits*: ";
    m_dealer.display_hand(cout);
    if( m_dealer.busted())
    {
      cout << " *bust*";
    }
    cout << endl;
  }
}

void Message::after_dealer_stands() const
{
  if( m_show)
  {
    if( !m_dealer.busted())
    {
      cout << "    dealer stands: ";
      m_dealer.display_hand(cout);
      cout << endl;
    }
  }
}

void Message::at_end_of_round() const
{
  if( m_show)
  {
    cout << endl;
    cout << "  Results";
    cout << endl;
    if( m_player.num_hands() > 1)
    {
      for( m_player.first_hand();
           m_player.has_more_hands();
           m_player.next_hand() )
      {
        cout << endl;
        cout << "    hand " << m_player.hand()+1 << " -- ";
        if( m_player.busted())
        {
          cout << "player lost  (busted)";
        }
        else if( m_dealer.busted())
        {
          cout << "player won!  (dealer busted)";
        }
        else
        {
          if( m_player.hand_value() < m_dealer.hand_value())
          {
            cout << "player lost  (";
            m_dealer.display_hand(cout);
            cout << "  > ";
            m_player.display_hand(cout);
            cout << ")";
          }
          else if( m_player.hand_value() > m_dealer.hand_value())
          {
            cout << "player won!  (";
            m_dealer.display_hand(cout);
            cout << "  < ";
            m_player.display_hand(cout);
            cout << ")";
          }
          else
          {
            cout << "*push*       (";
            m_dealer.display_hand(cout);
            cout << "  = ";
            m_player.display_hand(cout);
            cout << ")";
          }
        }
      }
    }
    else
    {
      cout << endl;
      if( m_player.busted())
      {
        cout << "    player lost  (busted)";
      }
      else if( m_dealer.busted())
      {
        cout << "    player won!  (dealer busted)";
      }
      else
      {
        if( m_player.hand_value() < m_dealer.hand_value())
        {
          cout << "    player lost  (";
          m_dealer.display_hand(cout);
          cout << "  > ";
          m_player.display_hand(cout);
          cout << ")";
        }
        else if( m_player.hand_value() > m_dealer.hand_value())
        {
          cout << "    player won!  (";
          m_dealer.display_hand(cout);
          cout << "  < ";
          m_player.display_hand(cout);
          cout << ")";
        }
        else
        {
          cout << "    *push*       (";
          m_dealer.display_hand(cout);
          cout << "  = ";
          m_player.display_hand(cout);
          cout << ")";
        }
      }
    }
    cout << endl;
    cout << endl;
    cout << "  Winnings: $"
         << m_player.winnings()
         << endl;
    cout << endl;
    cout << endl;
    cout << "  Penetration"
         << ": " << m_shoe.penetration()
         << " (" << m_shoe.penetration_ratio() << ")"
         << endl;
    cout << endl;
    cout << string(72,'-') << endl;
  }
}

void Message::before_shoe_reset() const
{
  if( m_show)
  {
    m_shoe.display(cout);
    cout << endl;
    if( m_scribe.says_to_start_a_new_game())
    {
      cout << "  Shoe is being reset for the start of a new game." << endl;
    }
    else
    {
      cout << "  Shoe is being reset:  penetration ratio"
           << " exceeded "
           << m_rules.max_penetration_ratio()
           << " threshhold." << endl;
    }
    cout << endl;
  }
}

void Message::after_shoe_reset() const
{
  if( m_show)
  {
    m_shoe.display(cout);
    cout << string(72,'-') << endl;
  }
}

void Message::when_player_gets_blackjack() const
{
  if( m_show)
  {
    cout << "  player *** Blackjack *** ";
    cout << endl;
    cout << endl;
  }
}

void Message::when_dealer_gets_blackjack() const
{
  if( m_show)
  {
    cout << "  dealer: ";
    m_dealer.display_hand(cout);
    cout << endl;
    cout << "  dealer *** Blackjack *** ";
    cout << endl;
    cout << endl;
  }
}

}

namespace dthorne0_blackjack { // Scribe
//##############################################################################

bool Scribe::says_to_keep_playing() const
{
  return m_num_rounds_total < m_num_rounds_to_play;
}

void Scribe::prepares_for_new_game()
{
  if( m_start_a_new_game)
  {
    m_num_rounds_this_game = 0;
    m_num_hands_this_game = 0;
    m_num_games++;
    m_start_a_new_game = false;
  }
}

void Scribe::prepares_for_new_round()
{
  // TODO:
}

bool Scribe::says_to_start_a_new_game() const
{
  return m_start_a_new_game;
}

bool Scribe::sees_that_player_has_blackjack() const
{
  if( m_player.num_hands()==1 && m_player.num_cards()==2)
  {
    if( m_player.hand_value() == 21)
    {
      return true;
    }
  }
  return false;
}

bool Scribe::player_lost_all_their_money() const
{
  return m_player.winnings() <= 0.0;
}

void Scribe::make_more_space_if_necessary()
{
  if( m_win_counts.size() < num_hands())
  {
    cerr << __FILE__ << " " << __LINE__ << " -- " << "BOOM!" << endl;
    exit(1);
  }
  if( m_hand_counts.size() < num_hands())
  {
    cerr << __FILE__ << " " << __LINE__ << " -- " << "BOOM!" << endl;
    exit(1);
  }
  if( m_round_counts.size() < num_hands())
  {
    cerr << __FILE__ << " " << __LINE__ << " -- " << "BOOM!" << endl;
    exit(1);
  }
  if( m_bankroll.size() < num_rounds())
  {
    cerr << __FILE__ << " " << __LINE__ << " -- " << "BOOM!" << endl;
    exit(1);
  }

  if( m_win_counts.size() <= num_hands()) { m_win_counts.push_back(0);}
  if( m_hand_counts.size() <= num_hands()) { m_hand_counts.push_back(0);}
  if( m_round_counts.size() <= num_rounds()) { m_round_counts.push_back(0);}
  if( m_bankroll.size() <= num_rounds()) { m_bankroll.push_back(0);}
  if( m_bankroll_up_counts.size() <= num_rounds())
  { m_bankroll_up_counts.push_back(0);}
  if( m_bankroll_down_counts.size() <= num_rounds())
  { m_bankroll_down_counts.push_back(0);}

}

void Scribe::prepares_to_record_results()
{
  m_num_hands_this_game++;

  m_num_rounds_this_game++;
  m_num_rounds_total++;

  m_round_counts[num_rounds()]++;

  make_more_space_if_necessary();

  if( m_num_hands_this_game > m_max_hands_in_a_single_game)
  {
    m_max_hands_in_a_single_game = m_num_hands_this_game;
  }

  if( m_num_rounds_this_game > m_max_rounds_in_a_single_game)
  {
    m_max_rounds_in_a_single_game = m_num_rounds_this_game;
  }
}

void Scribe::records_that_the_player_busted()
{
  m_hand_counts[num_hands()]++;
  m_num_hands_total++;

  m_num_losses_total++;

  if( m_player.num_hands() > 1)
  {
    if( m_player.cur_hand() > 1)
    {
      m_bankroll[num_rounds()] -= m_player.wager();
    }
    else
    {
      m_bankroll[num_rounds()] = m_bankroll[num_rounds()-1] - m_player.wager();
    }
  }
  else
  {
    m_bankroll[num_rounds()] = m_bankroll[num_rounds()-1] - m_player.wager();
  }
}

void Scribe::records_that_the_player_won()
{
  m_win_counts[num_hands()]++;

  m_hand_counts[num_hands()]++;
  m_num_hands_total++;

  m_num_wins_total++;

  if( m_player.num_hands() > 1)
  {
    if( m_player.cur_hand() > 1)
    {
      m_bankroll[num_rounds()] += m_player.wager();
    }
    else
    {
      m_bankroll[num_rounds()] = m_bankroll[num_rounds()-1] + m_player.wager();
    }
  }
  else
  {
    m_bankroll[num_rounds()] = m_bankroll[num_rounds()-1] + m_player.wager();
  }

}

void Scribe::records_that_the_player_lost()
{
  m_hand_counts[num_hands()]++;
  m_num_hands_total++;

  m_num_losses_total++;

  if( m_player.num_hands() > 1)
  {
    if( m_player.cur_hand() > 1)
    {
      m_bankroll[num_rounds()] -= m_player.wager();
    }
    else
    {
      m_bankroll[num_rounds()] = m_bankroll[num_rounds()-1] - m_player.wager();
    }
  }
  else
  {
    m_bankroll[num_rounds()] = m_bankroll[num_rounds()-1] - m_player.wager();
  }
}

void Scribe::records_a_push()
{
  m_hand_counts[num_hands()]++;
  m_num_hands_total++;

  m_num_pushes_total++;

  m_bankroll[num_rounds()] = m_bankroll[num_rounds()-1];
}

void Scribe::records_that_the_player_got_blackjack()
{
  m_win_counts[num_hands()]++;
  m_num_wins_total++;
  m_num_blackjacks_total++;

  m_hand_counts[num_hands()]++;
  m_num_hands_total++;

  m_bankroll[num_rounds()] = m_bankroll[num_rounds()-1] + 1.5*m_player.wager();
}

void Scribe::considers_the_results()
{
  if( m_player.bankroll_is_up())
  {
    m_bankroll_up_counts[num_rounds()]++;
  }
  else if( m_player.bankroll_is_down())
  {
    m_bankroll_down_counts[num_rounds()]++;
  }

  if( player_lost_all_their_money())
  {
    m_start_a_new_game = true;
  }
}

void Scribe::summarizes_results_to_stdout()
{
  cout << "  total games played : " << setw(12) << m_num_games << endl;
  cout << "  total rounds played: " << setw(12) << m_num_rounds_total << endl;
  cout << "  total hands played : " << setw(12) << m_num_hands_total
       << " (averaging "
       << (double)m_num_hands_total / m_num_games
       << " hands per game)"
       << endl;
  cout << endl;

  cout << "  total hands won    : " << setw(12) << m_num_wins_total << endl;
  cout << "  total hands lost   : " << setw(12) << m_num_losses_total << endl;
  cout << "  total hands pushed : " << setw(12) << m_num_pushes_total << endl;
  cout << "              ------------------------" << endl;
  cout << "                 sum : " << setw(12)
       << m_num_wins_total
        + m_num_losses_total
        + m_num_pushes_total
       << endl;
  if( m_num_hands_total != m_num_wins_total
                         + m_num_losses_total
                         + m_num_pushes_total)
  {
    cout << "                         |" << endl;
    cout << "                         |"
         << " WARNING: Hand tally does not match win/loss/push tally!"
         << endl;
    cout << "                         |" << endl;
    cout << "  total hands played : " << m_num_hands_total << endl;
  }

  cout << endl;
  cout << "          blackjacks : " << setw(12) << m_num_blackjacks_total << endl;

  cout << endl;
  cout.width(12);
  cout << "  win ratio          : "
       << (double)m_num_wins_total / m_num_hands_total
       << endl;
  cout << "  loss ratio         : "
       << (double)m_num_losses_total / m_num_hands_total
       << endl;
  cout << "  push ratio         : "
       << (double)m_num_pushes_total / m_num_hands_total
       << endl;
  cout << "  blackjack ratio    : "
       << (double)m_num_blackjacks_total / m_num_hands_total
       << endl;

  cout << endl;
  cout << "  win ratio disregarding pushed hands : "
       << (double)m_num_wins_total / ( m_num_hands_total - m_num_pushes_total)
       << endl;
  cout << "  loss ratio disregarding pushed hands: "
       << (double)m_num_losses_total / ( m_num_hands_total - m_num_pushes_total)
       << endl;
  cout << "  blackjack ratio disregarding pushed : "
       << (double)   m_num_blackjacks_total
                 / ( m_num_hands_total - m_num_pushes_total)
       << endl;
  cout << "  ratio of wins due to blackjack      : "
       << (double)m_num_blackjacks_total / m_num_wins_total
       << endl;
}

void Scribe::writes_results_to_matlab_scripts()
{
  ofstream fout;
  fout.open("results.m");

  int i;

  fout << endl;

  fout << "num_wins_total = " << m_num_wins_total << endl;
  fout << "num_losses_total = " << m_num_losses_total << endl;
  fout << "num_pushes_total = " << m_num_pushes_total << endl;
  fout << "num_hands_total = " << m_num_hands_total << endl;
  fout << "num_wins_total + num_losses_total + num_pushes_total" << endl;

  fout << "num_blackjacks_total = " << m_num_blackjacks_total << endl;

  fout << endl;
  fout << "  win_ratio      = "
       << (double)m_num_wins_total / m_num_hands_total
       << endl;
  fout << "  loss_ratio     = "
       << (double)m_num_losses_total / m_num_hands_total
       << endl;
  fout << "  push_ratio     = "
       << (double)m_num_pushes_total / m_num_hands_total
       << endl;
  fout << "  blackjack_ratio= "
       << (double)m_num_blackjacks_total / m_num_hands_total
       << endl;

  fout << endl;
  fout << "  win_ratio_disregarding_pushed_hands = "
       << (double)m_num_wins_total / ( m_num_hands_total - m_num_pushes_total)
       << endl;
  fout << "  loss_ratio_disregarding_pushed_hands= "
       << (double)m_num_losses_total / ( m_num_hands_total - m_num_pushes_total)
       << endl;
  fout << "  blackjack_ratio_disregarding_pushed = "
       << (double)   m_num_blackjacks_total
                 / ( m_num_hands_total - m_num_pushes_total)
       << endl;
  fout << "  ratio_of_wins_due_to_blackjack      = "
       << (double)m_num_blackjacks_total / m_num_wins_total
       << endl;
  fout << endl;

  fout << "win_counts = [";
  for( i=0; i<=m_max_hands_in_a_single_game; i++)
  {
    fout << " " << m_win_counts[i];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(win_counts);" << endl;
  fout << "title('win\\_counts')" << endl;
  fout << "xlabel('hand')" << endl;

  fout << endl;

  fout << "bankroll_cumulative = [";
  for( i=0; i<=m_max_rounds_in_a_single_game; i++)
  {
    fout << " " << m_bankroll[i];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(bankroll_cumulative);" << endl;
  fout << "title('bankroll\\_cumulative')" << endl;
  fout << "xlabel('round')" << endl;

  fout << endl;

  fout << "bankroll_up_counts = [";
  for( i=0; i<=m_max_rounds_in_a_single_game; i++)
  {
    fout << " " << m_bankroll_up_counts[i];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(bankroll_up_counts);" << endl;
  fout << "title('bankroll up counts')" << endl;
  fout << "xlabel('round')" << endl;
  fout << endl;

  fout << "bankroll_down_counts = [";
  for( i=0; i<=m_max_rounds_in_a_single_game; i++)
  {
    fout << " " << m_bankroll_down_counts[i];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(bankroll_down_counts);" << endl;
  fout << "title('bankroll down counts')" << endl;
  fout << "xlabel('round')" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(bankroll_up_counts./bankroll_down_counts);" << endl;
  fout << "title('bankroll up/down count ratios')" << endl;
  fout << "xlabel('round')" << endl;

  fout << endl;

  fout << "round_counts = [";
  for( i=0; i<=m_max_rounds_in_a_single_game; i++)
  {
    fout << " " << m_round_counts[i];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(round_counts);" << endl;
  fout << "title('round counts')" << endl;
  fout << "xlabel('round')" << endl;

  fout << endl;

  fout << "hand_counts = [";
  for( i=0; i<=m_max_hands_in_a_single_game; i++)
  {
    fout << " " << m_hand_counts[i];
  }
  fout << "];" << endl;
  fout << "figure;" << endl;
  fout << "plot(hand_counts);" << endl;
  fout << "title('hand counts')" << endl;
  fout << "xlabel('hand')" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(win_counts./hand_counts);" << endl;
  fout << "title('win ratios')" << endl;
  fout << "ylabel('win\\_counts./hand\\_counts')" << endl;
  fout << "xlabel('hand')" << endl;
  fout << "hnd=line([ 0 numel(win_counts)], win_ratio*[ 1 1]);" << endl;
  fout << "set(hnd,'color', 0.7*[ 1 1 1]);" << endl;
  fout << "set(hnd,'linestyle', ':');" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(bankroll_cumulative./round_counts);" << endl;
  fout << "title('average bankroll')" << endl;
  fout << "ylabel('bankroll\\_cumulative./round\\_counts')" << endl;
  fout << "xlabel('round')" << endl;

  fout << endl;

  fout << "figure;" << endl;
  fout << "plot(bankroll_up_counts./round_counts);" << endl;
  fout << "title('bankroll up ratios')" << endl;
  fout << "ylabel('bankroll\\_up\\_counts./round\\_counts')" << endl;
  fout << "xlabel('round')" << endl;

  fout << endl;

  fout.close();
}

//##############################################################################
}

