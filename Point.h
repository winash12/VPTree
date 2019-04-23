class Point {
 private:
  double coordinate1,coordinate2;
 public:
  virtual double  getCoordinate1();
  virtual double  getCoordinate2();
};

class SphericalPoint : public Point
{
 private:
  double longitude,latitude;
 public:
  double getCoordinate1();
  double getCoordinate2();
};
