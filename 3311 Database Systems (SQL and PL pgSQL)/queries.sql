-- COMP3311 22T3 Assignment 1
--
-- Fill in the gaps ("...") below with your code
-- You can add any auxiliary views/function that you like
-- The code in this file *MUST* load into an empty database in one pass
-- It will be tested as follows:
-- createdb test; psql test -f ass1.dump; psql test -f ass1.sql
-- Make sure it can load without error under these conditions


-- Q1: new breweries in Sydney in 2020

create or replace view Q1(brewery,suburb)
as
select B.name, L.town
from breweries B
	join locations L on (B.located_in=L.id)
where L.metro='Sydney' and B.founded=2020
;

-- Q2: beers whose name is same as their style

create or replace view Q2(beer,brewery)
as
select B.name, Bs.name
from beers B
	join styles S on (S.id=B.style)
	join brewed_by Bb on (Bb.beer=B.id)
	join breweries Bs on (Bs.id=Bb.brewery)
where S.name=B.name
;--Repeated beers are left because they're arguably distinct from each other.

-- Q3: original Californian craft brewery

create or replace view Q3Help
as
select B.name as name,  B.founded as founded
from breweries B
	join locations L on (B.located_in=L.id)
where L.region='California'
;

create or replace view Q3(brewery,founded)
as
select name, founded
from Q3Help
where founded=(select min(founded) from Q3Help)
;

-- Q4: all IPA variations, and how many times each occurs

create or replace view Q4(style,count)
as
select S.name, count(B.id)
from styles S
	join beers B on (B.style=S.id)
where S.name like '%IPA%'
group by S.name;
;

-- Q5: all Californian breweries, showing precise location

create or replace view Q5(brewery,location)
as
(select B.name, L.town
from breweries B
	join locations L on (B.located_in=L.id)
where L.region='California' and L.town is not null)
union
(select B.name, L.metro
from breweries B
	join locations L on (B.located_in=L.id)
where L.region='California' and L.town is null)
;

-- Q6: strongest barrel-aged beer

create or replace view Q6Help(beer,brewery,abv)
as
(select B.name, Bs.name, B.abv
from beers B
	join brewed_by Bb on (B.id=Bb.beer)
	join breweries Bs on (Bs.id=Bb.brewery)
where B.notes like '%barrel%aged%')
union
(select B.name, Bs.name, B.abv
from beers B
	join brewed_by Bb on (B.id=Bb.beer)
	join breweries Bs on (Bs.id=Bb.brewery)
where B.notes like '%aged%barrel%')
;

create or replace view Q6(beer,brewery,abv)
as
select beer, brewery, abv
from Q6Help
where abv=(select max(abv) from Q6Help)
;
-- Q7: most popular hop

create or replace view Q7Help(hop,count)
as
select I.name, count(I.name)
from ingredients I
	join contains C on (C.ingredient=I.id)
where I.itype='hop'
group by I.name
;

create or replace view Q7(hop)
as
select hop
from Q7Help
where count=(select max(count) from Q7Help)
;

-- Q8: breweries that don't make IPA or Lager or Stout (any variation thereof)

create or replace view Q8(brewery)
as
(select Bs.name
from breweries Bs)
except
(select Bs.name
from breweries Bs
	join brewed_by Bb on (Bb.brewery=Bs.id)
	join beers B on (Bb.beer=B.id)
	join styles S on (S.id=B.style)
where S.name like '%IPA%' or S.name like '%Lager%' or S.name like '%Stout%')
;

-- Q9: most commonly used grain in Hazy IPAs

create or replace view Q9Help(grain,count)
as
select I.name, count(I.name)
from ingredients I
	join contains C on (I.id=C.ingredient)
	join beers B on (B.id=C.beer)
	join styles S on (S.id=B.style)
where S.name='Hazy IPA' and I.itype='grain'
group by I.name
;

create or replace view Q9(grain)
as
select grain
from Q9Help
where count=(select max(count) from Q9Help)
;

-- Q10: ingredients not used in any beer

create or replace view Q10(unused)
as
(select I.name
from ingredients I)
except
(select I.name
from ingredients I
	join contains C on (C.ingredient=I.id))
;

-- Q11: min/max abv for a given country

drop type if exists ABVrange cascade;
create type ABVrange as (minABV float, maxABV float);

create or replace function
	Q11(_country text) returns ABVrange
as $$
declare
	range ABVrange;
begin
	select min(B.ABV)::numeric(4,1), max(B.ABV)::numeric(4,1)
	into range
	from beers B
		join brewed_by Bb on (Bb.beer=B.id)
		join breweries Bs on (Bs.id=Bb.brewery)
		join locations L on (L.id=Bs.located_in)
	where L.country=_country;
	if (not found) then
		raise notice 'NOT FOUND';
		return (0,0)::ABVrange;
	else
		raise notice 'found = %, range = %', found, range;
		return range;
	end if;
end;
$$
language plpgsql;

-- Q12: details of beers

drop type if exists BeerData cascade;
create type BeerData as (beer text, brewer text, info text);

create or replace function
	Q12(partial_name text) returns setof BeerData
as $$
declare
	bev text;
	brew text;
	inf text;
	res record;
begin	
	for res in
		select B.name as beer, Bs.name as brewer, B.id as id
		from beers B
			join brewed_by Bb on (B.id=Bb.beer)
			join breweries Bs on (Bs.id=Bb.brewery)
		where B.name like '%'||partial_name||'%';
	loop
		select I.name
		from res
			join contains C on (C.beer=res.id)
			join ingredients I on (I.id=C.ingredient)
		where I.itype='hop'
end;
$$
language plpgsql;
