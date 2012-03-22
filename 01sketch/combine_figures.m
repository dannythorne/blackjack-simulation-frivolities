function []=combine_figures(figs)

num_figs = numel(figs);

n = zeros(num_figs);
nmax = 0;

for i=1:num_figs
  figure(figs(i));
  hnd = get(gca,'children');
  x = get(hnd(1),'ydata');
  n(i) = numel(x);
  if( n(i) > nmax)
    nmax = n(i);
  end
end

x = zeros(num_figs,nmax);

for i=1:num_figs
  figure(figs(i));
  hnd = get(gca,'children');
  temp = get(hnd(1),'ydata');
  x(i,1:n(i)) = temp;
end

figure;
hold on;
for i=1:num_figs
  hnd=plot(x(i,1:n(i)));
  w = (i-1)/(num_figs-1);
  set(hnd,'color', (w)*[ 1 0 0] + (1-w)*[ 0 0 1]);
end

end
