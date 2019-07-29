#ifndef POINT_H
#define POINT_H

class  Point
{
 private:
  double coordinate1,coordinate2;
 public:
  virtual double  getCoordinate1() const;
  virtual double  getCoordinate2() const ;
  virtual void    setCoordinate1(double coordinate1);
  virtual void    setCoordinate2(double coordinate2);
};

class SphericalPoint : public Point
{
 private:
  double longitude,latitude;
 public:
  double getCoordinate1()  const;
  double getCoordinate2()  const ;
  void   setCoordinate1(double latitude);
  void   setCoordinate2(double longitude);
};

#endif /* POINT_H */
