SELECT *
FROM `avrdb`.`avrtemp` 
where date_time >= DATE_SUB(NOW(), INTERVAL 1 HOUR);

SELECT date_time, temp FROM `avrdb`.`avrtemp` where channel=1 AND date_time >= DATE_SUB(NOW(), INTERVAL 1 HOUR);

SELECT DISTINCT date_time from 
`avrdb`.`avrtemp` 
where date_time >= DATE_SUB(NOW(), INTERVAL 1 HOUR);

SELECT * from 
`avrdb`.`avrtemp` 
where date_time >= DATE_SUB(NOW(), INTERVAL 1 HOUR) 
GROUP BY date_time;

SELECT TIME(date_time) as time1, temp*.1, humidity
FROM `avrdb`.`avrtemp` 
where channel=1 AND 
date_time >= DATE_SUB(NOW(), INTERVAL 1 HOUR);

Select temp*.1 as temp1, humidity 
FROM `avrdb`.`avrtemp` 
WHERE channel=1 
ORDER by date_time DESC LIMIT 1;

Select temp*.1 as temp1,humidity,channel 
FROM `avrtemp` 
WHERE channel=2 
ORDER by date_time DESC LIMIT 1;

SELECT channel,temp*.1 as temp1,humidity, 
GROUP_CONCAT(date_time)
FROM `avrdb`.`avrtemp` 
GROUP BY date_time;

SELECT channel,temp*.1 as temp1,humidity,
date_time,
DATE(date_time) as date1, 
TIME(date_time) as time1,
DATE_FORMAT(date_time, '%Y-%c-%d %H:%i') as datef,
ROUND(TIME_TO_SEC(date_time)/60) as dtime
FROM `avrdb`.`avrtemp` 
ORDER BY date_time DESC;

Select date_time 
FROM `avrdb`.`avrtemp` 
ORDER by date_time DESC LIMIT 1;

#see http://stackoverflow.com/questions/940550/computing-average-values-over-sections-of-date-time
SELECT  
MONTH(date_time),
DAY(date_time), 
HOUR(date_time), 
MINUTE(date_time), 
AVG(temp)*.1 as temp1
FROM `avrdb`.`avrtemp` 
WHERE channel=1
GROUP BY
MONTH(date_time),
DAY(date_time), 
HOUR(date_time), 
MINUTE(date_time) 
WITH ROLLUP;

DELETE 
FROM `avrdb`.`avrtemp` 
WHERE id<14;

#query average temp for hours
SELECT month1, day1, hour1, temp1 FROM 
(SELECT  
MONTH(date_time) as month1,
DAY(date_time) as day1, 
HOUR(date_time) as hour1, 
MINUTE(date_time) as minutes, 
AVG(temp)*.1 as temp1
FROM `avrdb`.`avrtemp` 
WHERE channel=1
GROUP BY
MONTH(date_time),
DAY(date_time), 
HOUR(date_time), 
MINUTE(date_time) 
WITH ROLLUP) as myroundup
WHERE minutes is NULL AND hour1 is NOT NULL
ORDER BY month1 DESC, day1 DESC, hour1 ASC LIMIT 24
;

#query average temp for hours
SELECT month1, day1, hour1, temp1 FROM 
(SELECT  
MONTH(date_time) as month1,
DAY(date_time) as day1, 
DATE_FORMAT(date_time,'%H') as hour1, 
MINUTE(date_time) as minutes, 
AVG(temp)*.1 as temp1
FROM `avrdb`.`avrtemp` 
WHERE channel=1
GROUP BY
MONTH(date_time),
DAY(date_time), 
HOUR(DATE_FORMAT(date_time,'%T')), 
MINUTE(date_time) 
WITH ROLLUP) as myroundup
WHERE minutes is NULL AND hour1 is NOT NULL
ORDER BY month1 DESC, day1 DESC, hour1 ASC LIMIT 24
;

#by days
SELECT * FROM (
SELECT channel,
DAY(date_time) as day1, 
AVG(temp)*.1 as temp1,
date_time
FROM `avrdb`.`avrtemp` 
WHERE channel=1
GROUP BY
day1
WITH ROLLUP
) as myroundup
WHERE day1 is not null
ORDER BY date_time ASC;


#by days
select * from (
SELECT channel,
DAY(date_time) as day1, 
HOUR(date_time) as hour1,
AVG(temp)*.1 as temp1
FROM `avrdb`.`avrtemp` 
WHERE channel=1 AND
DATEDIFF(now(), date_time)<8 
GROUP BY
day1, hour1
WITH ROLLUP) as myroundup
where hour1 is NULL
;

#query average temp for days
SELECT channel, month1, day1, temp1 FROM 
(SELECT  
MONTH(date_time) as month1,
DAY(date_time) as day1, 
HOUR(date_time) as hour1, 
MINUTE(date_time) as minutes, 
AVG(temp)*.1 as temp1,
channel
FROM `avrdb`.`avrtemp` 
WHERE channel=1
GROUP BY
MONTH(date_time),
DAY(date_time), 
HOUR(date_time), 
MINUTE(date_time) 
WITH ROLLUP) as myroundup
WHERE hour1 is NULL AND day1 is NOT NULL
;

#by days
SELECT * FROM (
	SELECT * FROM (
		SELECT channel,
		DAY(date_time) as day1, 
		AVG(temp)*.1 as temp1,
		date_time,
		DATE_FORMAT(date_time, '%j') as dayofyear
		FROM `avrdb`.`avrtemp` 
		WHERE channel=1 
		GROUP BY
		dayofyear
		WITH ROLLUP
		) as myroundup
	WHERE day1 is not null
	AND DATE_FORMAT(now(), '%j') - dayofyear < 10 
	ORDER BY date_time ASC) as mydays
order by date_time desc
;

#by hours
SELECT * FROM (
SELECT * FROM (
SELECT channel,
DATE_FORMAT(date_time,'%H') as hour1,
DAY(date_time) as day1, 
AVG(temp)*.1 as temp1,
date_time
FROM `avrdb`.`avrtemp` 
WHERE channel=1
GROUP BY
day1, hour1
WITH ROLLUP
) as myroundup
WHERE day1 is not null AND hour1 is NOT NULL
ORDER BY date_time DESC LIMIT 24
) as sorted_hours
order by date_time asc;

