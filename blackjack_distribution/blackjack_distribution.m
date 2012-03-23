function [ratio_losing_streaks] = ...
  p(nmax,max_length_of_losing_streak,initial_winnings,initial_wager)

track_losing_streaks = 1;
track_even_odds_for_comparison = 1;
track_winnings = (initial_wager>0);

if ~track_losing_streaks
  ratio_losing_streaks = 0;
end
if track_winnings
  average_winnings = zeros(nmax,1);
  average_positive_winnings = zeros(nmax,1);
  ratio_wins = zeros(nmax,1);
  win_count = zeros(nmax,1);
  weighted_num_wins = zeros(nmax,1);
end

pwin_raw = 0.433322; % 0.424249;
ploss_raw = 0.481628; % 0.490964;

pwin = 0.469214; %pwin_raw / ( pwin_raw + ploss_raw);
ploss = 1 - pwin; %ploss_raw / ( pwin_raw + ploss_raw);

if track_losing_streaks
  ratio_losing_streaks = zeros(nmax,max_length_of_losing_streak);
  if track_even_odds_for_comparison
    eo_ratio_losing_streaks = zeros(nmax,max_length_of_losing_streak);
  end
end

tic
t = 0;

for n=1:nmax % num rounds per game

  m = 2^n; % number of rounds to play

  psum = 0;
  if track_losing_streaks
    ratio_losing_streaks(n,1) = n;
    if track_even_odds_for_comparison
      eo_ratio_losing_streaks(n,1) = n;
    end
  end
  if track_winnings
    num_wins = 0;
    positive_winnings_sum = 0;
  end

  for i=1:m % games
    %s = '';
    k = i-1;
    p = 1;
    if track_even_odds_for_comparison
      eo_p = 1;
    end
    if track_losing_streaks
      losing_streak = 0;
      max_losing_streak = 0;
    end

    if track_winnings
      wager = initial_wager;
      winnings = initial_winnings;
    end

    for j=1:n % rounds
      d = mod(k,2);

      if d==1 %-----------------------------------------------------------------

        p = p*pwin;

        if track_winnings
          if wager > 0
            winnings = winnings + wager;

            if winnings > initial_wager
              wager = initial_wager;
            else
              error('This should never happen, right?');
              wager = winnings;
            end
          end
          if( winnings > 0)
            win_count(j) = win_count(j) + 1;
          end
        end
        if track_even_odds_for_comparison
          eo_p = eo_p*0.5;
        end
        if track_losing_streaks
          if losing_streak > max_losing_streak
            max_losing_streak = losing_streak;
          end
          losing_streak = 0;
        end

      else %--------------------------------------------------------------------

        p = p*ploss;

        if track_winnings

          winnings = winnings - wager;

          if winnings > 0

            if winnings > 2*wager
              wager = 2*wager;
            else
              wager = winnings;
            end

          else
            wager = 0;

            if winnings ~= 0
              error(sprintf('ERROR: Winnings %f went negative!',winnings));
            end
          end
        end
        if track_even_odds_for_comparison
          eo_p = eo_p*0.5;
        end
        if track_losing_streaks
          losing_streak = losing_streak + 1;
        end

      end %---------------------------------------------------------------------

      k = floor(k/2);

    end % for j (rounds)

    psum = psum + p;

    if track_winnings

      average_winnings(n) = average_winnings(n) + winnings*p;

      if winnings > initial_winnings
        num_wins = num_wins + 1;
        weighted_num_wins(n) = weighted_num_wins(n) + p;
        positive_winnings_sum = positive_winnings_sum + winnings;
      end
    end

    if track_losing_streaks

      if( losing_streak > max_losing_streak)
        max_losing_streak = losing_streak;
      end

      if( max_losing_streak > max_length_of_losing_streak)
        max_losing_streak = max_length_of_losing_streak;
      end

      if max_losing_streak > 1
        for j=2:max_losing_streak
          ratio_losing_streaks(n,j) = ratio_losing_streaks(n,j) + p;
          if track_even_odds_for_comparison
            eo_ratio_losing_streaks(n,j) = eo_ratio_losing_streaks(n,j) + eo_p;
          end
        end
      end
    end

  end % for i (games)

  disp(sprintf('psum = %f',psum));
  if track_losing_streaks
    disp(sprintf('%f ', ratio_losing_streaks(n,:)));
  else
    disp(sprintf('n = %d',n));
  end
  if track_winnings
    disp(sprintf('average winnings = %20.17f',average_winnings(n)));
    average_positive_winnings(n) = positive_winnings_sum/num_wins;
    disp(sprintf('average positive winnings = %20.17f', ...
      average_positive_winnings(n)));
    ratio_wins(n) = num_wins/m;
    disp(sprintf('win %d of %d games (ratio = %f)',num_wins,m,ratio_wins(n)));
  end

  tnext = toc;
  disp(sprintf( ...
    'elapsed time %f seconds (%f minutes), %f seconds (%f minutes) since last toc', ...
    tnext,tnext/60,(tnext-t),(tnext-t)/60));
  disp('------------');
  t = tnext;

end % for n (num rounds per game)

if track_losing_streaks
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  figure;
  set(gcf,'position',[ 2 278 560 420]);
  title(sprintf('Blackjack, P(win round)=%f, P(lose round)=%f',pwin,ploss));
  hold on;
  for i=2:max_length_of_losing_streak
    x = find(ratio_losing_streaks(:,i)>0);
    hnd=plot(x,ratio_losing_streaks(x,i));
    set(hnd,'color', ...
      (1-(i-2)/(max_length_of_losing_streak-2))*[ 0 0 1] ...
    + ((i-2)/(max_length_of_losing_streak-2))*[ 1 0 0] ...
    );
    c{i-1} = sprintf('P(at least %d round losing streak)',i);
  end
  xlabel('number of rounds');

  if track_even_odds_for_comparison
    for i=2:max_length_of_losing_streak
      x = find(eo_ratio_losing_streaks(:,i)>0);
      hnd=plot(x,eo_ratio_losing_streaks(x,i));
      set(hnd,'color', 0.7*[ 1 1 1]);
      if i==2
        c{max_length_of_losing_streak} = ...
          sprintf('even odds curves for comparison');
        legend(c,'location','best');
      end
    end
  else
    legend(c,'location','best');
  end

  xlim = get(gca,'xlim');
  if xlim(1) < 1
    disp(sprintf('xlim(1) = %f < 1 <-- Why?',xlim(1)));
  end
  if xlim(2) > nmax
    disp(sprintf('xlim(2) = %f > nmax = %d <-- Why?',xlim(2),nmax));
  end
  set(gca,'xlim',[1 nmax]);

  ylim = get(gca,'ylim');
  if ylim(1) ~= 0
    disp(sprintf('ylim(1) = %f <-- Why?',ylim(1)));
  end
  if ylim(2) > 1
    disp(sprintf('ylim(2) = %f > 1 <-- Why?',ylim(2)));
  end
  set(gca,'ylim',[0 1]);

  filename = sprintf('losing_streak_probabilities_nmax_%d_max_losing_streak_to_track_%d_initial_amount_%d_base_wager_%d.png',nmax,max_length_of_losing_streak,initial_winnings,initial_wager);
  disp(sprintf('Writing file \"%s\".',filename));
  print(filename,'-dpng','-r128');
end

if track_winnings
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  figure;
  set(gcf,'position',[ 568 278 560 420]);
  hold on;
  hnd=plot(weighted_num_wins);
  set(hnd,'color', 0.5*[ 1 1 1]);
  hnd=plot(ratio_wins);
  title(sprintf('ratio of wins, initial amount=%d, base wager=%d',initial_winnings,initial_wager));
  xlabel('number of rounds');
  set(gca,'ylim',[ min(.5,min(ratio_wins)) 1]);
  filename = sprintf('ratio_of_wins_nmax_%d_initial_amount_%d_base_wager_%d.png',nmax,initial_winnings,initial_wager);
  disp(sprintf('Writing file \"%s\".',filename));
  print(filename,'-dpng','-r128');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  figure;
  hold on;
  title(sprintf('average winnings, initial amount=%d, base wager=%d',initial_winnings,initial_wager));
  hnd=plot(average_winnings);
  hnd=plot(average_positive_winnings);
  set(hnd,'color', 0.7*[ 1 1 1]);
  hnd=plot(average_positive_winnings.*ratio_wins);
  set(hnd,'color', 0.7*[ 1 0 0]);
  legend('average winnings', ...
         'average positive winnings', ...
         'average positive winnings * ratio wins', 'location', 'best');
  filename = sprintf('average_winnings_nmax_%d_initial_amount_%d_base_wager_%d.png',nmax,initial_winnings,initial_wager);
  disp(sprintf('Writing file \"%s\".',filename));
  print(filename,'-dpng','-r128');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  figure;
  hold on;
  plot(win_count);
  title('win count');
  xlabel('num rounds');

end

