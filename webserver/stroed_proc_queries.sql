#using a stored proc for interval average
#http://stackoverflow.com/questions/4073242/calculate-averages-for-5-min-intervals-in-mysql
#gegerate a table with timestamps with stepping from
# iFrom to iTo with interval iStep(minutes)
USE avrdb;
DELIMITER $$
CREATE PROCEDURE generate_series (
  iFrom TIMESTAMP, iTo TIMESTAMP, iStep INTEGER )

body:
BEGIN

  DROP TEMPORARY TABLE IF EXISTS stamp_series;
  CREATE TEMPORARY TABLE stamp_series 
    ( stamp TIMESTAMP NOT NULL);

  IF iFrom IS NULL OR iTo IS NULL OR iStep IS NULL
    THEN LEAVE body; 
  END IF;

  SET @iMax = iFrom;
  SET @iMin = iTo;

  InsertLoop: LOOP
    INSERT INTO stamp_series SET stamp = @iMax;
    SET @iMax = DATE_SUB( @iMax, INTERVAL iStep SECOND);
    IF @iMin > @iMax 
      THEN LEAVE InsertLoop; 
    END IF;
  END LOOP;

END; $$
DELIMITER ;

CALL generate_series( 
  DATE_SUB(NOW(), INTERVAL 1 DAY),
  NOW() , 
  5*60);

SELECT 
  stamp, 
  DATE_SUB(stamp, INTERVAL 5 MINUTE) AS stamp_min_5, 
( 
  SELECT COALESCE( AVG(temp), 0) 
  FROM `avrdb`.`avrtemp` 
  WHERE `channel` = 2 AND 
  date_time <= stamp AND 
  date_time > DATE_SUB(stamp, INTERVAL 5 MINUTE) 
) 
AS average_temp FROM stamp_series; 
