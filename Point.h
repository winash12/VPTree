class Point {
 private:
  double coordinate1,coordinate2;
 public:
  double  getCoordinate1();
  double  getCoordinate2();
  void    setCoordinate1(double coordinate1);
  void    setCoordinate2(double coordinate2);
};

class SphericalPoint : public Point
{
 private:
  double longitude,latitude;
 public:
  double getCoordinate1();
  double getCoordinate2();
  void   setCoordinate1(double latitude);
  void   setCoordinate2(double longitude);
};

