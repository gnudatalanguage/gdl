PROGRAM test_bug_635

    IMPLICIT NONE

    integer(kind=4)       :: i, j
    integer(kind=4) , parameter      :: ncities=2
    real(kind=4)          :: data(5, 3)

    TYPE :: City
        integer(kind=4)       :: Population
        real(kind=4)          :: Latitude, Longitude    !! degrees north & west
        integer(kind=4)       :: Elevation              !! in meters
    END TYPE City


    TYPE(City) :: Hatley = City(674, 45.2, 72.9, 300)
    TYPE(City) :: group(ncities)

    DO i=1,ncities
        group(i) = Hatley
        group(i)%Population = 1000000
    ENDDO

    print *, Hatley%Population, " inhabitants"
    print *, group(2)%Population, " inhabitants"

    OPEN(1, FILE="test_bug_635.dat", FORM="unformatted")
      DO  j = 1, 3
        DO  i = 1, 5
          data(i,j) = ((j - 1) * 5) + (i - 1)
        ENDDO
      ENDDO

      WRITE(1) data
      WRITE(1) group(1:ncities)
   CLOSE(1)

END PROGRAM test_bug_635
