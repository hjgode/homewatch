use avrdb;

select pwrid, 
ROUND(usedws/60/60/1000) as usedkwh,
ontime*100/total as ontimes,
ROUND(total/60/60/24) as totaldays  
from powerusage 
where pwrid='22F0' 
order by pwrid, date_time limit 40;

select pwrid, 
ROUND(usedws/60/60/1000) as usedkwh,
ontime*100/total as ontimes,
ROUND(total/60/60/24) as totaldays  
from powerusage 
GROUP BY pwrid 
order by pwrid, date_time limit 40;

select distinct pwrid from powerusage;

select pwrid, count(*) from powerusage GROUP BY pwrid;

select pwrid,currentw,date_time 
from powerusage 
where currentw<2 
order by date_time DESC;

select pwrid,currentw,date_time 
from powerusage 
order by date_time DESC LIMIT 3;

SELECT usedws as minusedws,  
MIN(date_time) AS MinTimeStamp
FROM powerusage 
WHERE pwrid='1E0E';

SELECT usedws as maxusedws,
MAX(date_time) AS MaxTimeStamp
FROM powerusage
WHERE pwrid='1E0E';

select pwrid,
day,  
#sum(diff) as total_diff
ROUND(diff/60/60/1000, 2) as kwh
from (
    select 
    dayofyear(date_time) as day, 
    pwrid, 
    max(usedws) - min(usedws) as diff
    from `powerusage`
    where pwrid in ('1E0E','1B67', '22F0')
    group by day, pwrid
    ) a
group by pwrid, day;

select distinct pwrid from powerusage;

select pwrid, (max(usedws) - min(usedws))/60/60/1000 as diff
from powerusage
where pwrid='1E0E' and dayofyear(date_time)=133;

select id, pwrid, usedws, date_time 
from powerusage
where pwrid='1E0E' and dayofyear(date_time)=133
ORDER by date_time;

select distinct currentw 
from powerusage
order by currentw;

use avrdb;
update powerusage 
set status=false 
where currentw < 3;

select pwrid,
usedws,
date_time
from powerusage
group by date_time;


select pwrid,currentw,usedws,date_time
from powerusage
where day(date_time)=
(select day(max(date_time)) as tag from powerusage)
order by pwrid;

select *, day(date_time) as tag
from powerusage
order by date_time DESC
limit 10;

select a.pwrid, 
a.date_time, 
a.currentw,
(a.usedws-b.usedws) as diff
from powerusage a
CROSS JOIN powerusage b
where (day(b.date_time)=day(a.date_time)-1) AND
(day(a.date_time)=26)
group by a.pwrid
order by a.pwrid;

#percentage of on cycles
SELECT pwrid, COUNT(*) onoff_count,
    SUM(IF (currentw > 3,1,0)) on_count,  
    SUM(IF (currentw <= 3,1,0)) off_count,
DATEDIFF(MAX(date_time),MIN(date_time)) as timeinterval
FROM powerusage
GROUP by pwrid;

#number of measures
SELECT pwrid,
COUNT(*)
from powerusage
group by pwrid;

#set of stored dates
select distinct(date(date_time))
from powerusage;

select id, pwrid, usedws, date_time,
MAX(usedws) as max1, MIN(usedws) as min1,
(((MAX(usedws) - MIN(usedws))/60)/60)/1000 as kWh 
from powerusage
where pwrid='1E0E' and dayofyear(date_time)=133
ORDER by date_time;

select id, pwrid, usedws, date_time,
MAX(usedws) as max1
from powerusage
where pwrid='1E0E' and dayofyear(date_time)=133;

select id, pwrid, usedws, dayofyear(date_time)
from powerusage
where usedws=10022086532;

#get last measure
select id, pwrid, usedws, DATE_FORMAT(date_time, "%Y%m%d %H:%i:%S")
from powerusage
where pwrid='1E0E' and dayofyear(date_time)=133
ORDER BY id DESC
LIMIT 1;
 
#get first measure
select id, pwrid, usedws, DATE_FORMAT(date_time, "%Y%m%d %H:%i:%S")
from powerusage
where pwrid='1E0E' and dayofyear(date_time)=133
ORDER BY id ASC
LIMIT 1;

select id, pwrid, usedws, DATE_FORMAT(date_time, "%Y%m%d %H:%i:%S")
from powerusage
where pwrid='1E0E' and dayofyear(date_time)=133
ORDER BY id ASC
LIMIT 1;

select * 
from
(select id, usedws, DATE_FORMAT(date_time, "%Y%m%d %H:%i:%S")
 from powerusage 
 where pwrid='1E0E' and dayofyear(date_time)=133
 ORDER BY id DESC
 LIMIT 1) as table1;

