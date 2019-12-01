#include <iostream>
#include <map>

class Polygon {
  protected:
    int width, height;
  public:
    virtual int area() = 0;
    void set_values (int a, int b)
      { width=a; height=b; }
};

class Rectangle: public Polygon {
  public:
    int area()
      { return width*height; }
};

typedef std::shared_ptr<Polygon> PolygonPtr;

class Triangle: public Polygon {
  public:
    int area()
      { return width*height/2; }
};

class A {
    public:
        PolygonPtr p;
};

class PolygonFactory {
    public:
        static PolygonPtr PolygonFactory::CreatePolygon(std::string type) {
            polygon_map = {
                {"aaa", PolygonPtr(new Rectangle())},
                {"bbb", PolygonPtr(new Triangle())}
            };
            auto x = polygon_map.find(type); 
            if(x != polygon_map.end()) {
                return x->second;
            } else {
                return nullptr;
            }
        } 
        static std::map<std::string, PolygonPtr> polygon_map;
};

int main () {
    A a;
    a.p = PolygonFactory::CreatePolygon("aaa");
    a.p->set_values (4,5);
    std::cout << a.p->area() << '\n';
    return 0;
}
