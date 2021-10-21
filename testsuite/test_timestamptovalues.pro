pro test_timestamptovalues
timestamp_string = '2012-09-04T11:25:15-06:00'

        

timestamptovalues, timestamp_string, $
                   YEAR=year, MONTH=month, DAY=day, $
                   HOUR=hour, MINUTE=minute, $
                   SECOND=second, OFFSET=offset
PRINT, "TEST n°1"
PRINT, day, month, year, hour, minute, second, offset

timestamp_array = ['2012-09-04T11:25:15-06:00','2021-08-05','2019-12-08T14:43:22.54Z']

timestamptovalues, timestamp_array, $
                   YEAR=year, MONTH=month, DAY=day, $
                   HOUR=hour, MINUTE=minute, $
                   SECOND=second, OFFSET=offset

PRINT, "TEST n°2"
PRINT, day, month, year, hour, minute, second, offset

timestamp_test = timestamp()

timestamptovalues, timestamp_test, $
                   YEAR=year, MONTH=month, DAY=day, $
                   HOUR=hour, MINUTE=minute, $
                   SECOND=second, OFFSET=offset

PRINT, "TEST n°3"
PRINT, day, month, year, hour, minute, second, offset

end