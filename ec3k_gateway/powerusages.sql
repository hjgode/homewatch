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
where currentw>2 
order by date_time DESC;

select pwrid,currentw,date_time 
from powerusage 
order by date_time DESC LIMIT 3;
