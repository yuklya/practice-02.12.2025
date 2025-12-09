#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace topit {
  struct p_t {
    int x, y;
  };
  struct f_t {
    p_t aa, bb;
  };
  size_t rows(f_t fr);
  bool operator == (p_t a, p_t  b);
  bool operator!= (p_t a, p_t b);
  struct IDraw {
    virtual p_t next() const = 0;
    virtual p_t next(p_t prev) const = 0;
    virtual ~IDraw() = default;
  };
  struct Dot: IDraw {
    explicit Dot (p_t dd);
    p_t next() const override;
    p_t next(p_t prev) const override;
    p_t d;
  };
  struct Rest: IDraw {
    Rest( p_t pos, int w, int h);
    Rest (p_t a, p_t b);
    p_t begin() const override;
    p_t next(p_t prev) const override;
    f_t rect;
  };
}
  p_t* extend(const p_t * pts, size_t s, p_t fill);   
  void extend( p_t** pts, size_t& s, p_t fill);
  void append(const IDraw* sh, p_t** ppts, size_t& s);
  f_t frame(const p_t * pts, size_t s);
  char * canvas (f_t fr, char fill);
  void paint(p_t p, char * cnv, f_t fr, char fill);
  void flush(std::ostream& os, const char* cnv, f_t fr);
  size_t cols(f_t fr);
}

int main() {
  using namespace topit;
  int err = 0;
  IDraw* shp[3] = {};
  p_t * pts = nullptr;
  size_t s = 0;
  try {
    shp[0] = new Dot({0, 0});
    shp[1] = new Dot ({2, 3});
    shp[2] = new Dot({-5, -2});
    for (size_t i = 0; i< 3; ++i) {
      append (shp[i], &pts, s);
    }
    f_t fr = frame(pts, s);
    
    char * cnv = canvas(fr, '.');
    
    size_t width = cols(fr);
    for (size_t j = 0; j < width; ++j) {
      cnv[j] = '#'; 
    }
    
    
    flush(std::cout, cnv, fr);
    delete [] cnv;
  } catch (...) {
    std:: cerr<< "Error!\n";
    err = 1;
  }
  for (int i = 2; i >= 0; --i) {
    delete shp[i];
  }
  delete[] pts;
  return err;
}

topit::p_t* topit::extend(const p_t * pts, size_t s, p_t fill) { 
  p_t* r = new p_t [s+1];
  for (size_t i=0; i<s;++i) {
    r[i] = pts[i];
  }
  r[s] = fill;
  return r; 
} 

void topit::extend( p_t** pts, size_t& s, p_t fill){
  p_t* r = extend(*pts, s, fill);
  delete[] *pts;
  ++s;
  *pts = r;
}

void topit::append(const IDraw* sh, p_t** ppts, size_t& s) {
  extend(ppts, s, sh->next()); 
  p_t b = sh->next(); 
  while (sh->next(b) != sh->next()){
    b = sh->next(b);
    extend(ppts, s, b);
  }
}

void topit::paint(p_t p, char* cnv, f_t fr, char fill) {
  size_t dx = p.x - fr.aa.x;
  size_t dy = fr.bb.y - p.y;
  cnv[dy * cols(fr) + dx] = fill;
}

void topit::flush(std::ostream& os, const char* cnv, f_t fr){
  for (size_t i = 0; i < rows(fr); ++i){
    for (size_t j = 0; j < cols(fr); ++j){
      os << cnv[i * cols(fr) + j];
    }
  os<< "\n";
  }
}

char * topit::canvas(f_t fr, char fill){
  size_t sz = rows(fr) * cols(fr);
  char * c = new char[sz];
  for (size_t i = 0; i < sz; ++i) {
    c[i] = fill;
  }
  return c;
}

topit::p_t topit::Dot::next() const {
  return d;
}

topit::p_t topit::Dot::next(p_t prev) const {
  if (prev != d){
    throw std::logic_error("bad prev");
  }
  return d;
}

size_t topit::rows(f_t fr) {
  return (fr.bb.y - fr.aa.y + 1);
}

size_t topit::cols(f_t fr) {
  return (fr.bb.x - fr.aa.x + 1);
}

bool topit::operator == (p_t a, p_t  b) {
  return a.x == b.x && a.y == b.y;
}

bool topit::operator!= (p_t a, p_t b) {
  return !(a == b);
}

topit::f_t topit::frame(const p_t* pts, size_t s)
{
  int minx = pts[0].x,miny = pts[0].y;
  int maxx = minx, maxy = miny;
  for (size_t i = 1; i<s; ++i){
    minx = std::min(minx, pts[i].x);
    miny = std::min(miny, pts[i].y);
    maxx = std::max(maxx, pts[i].x);
    maxy = std::max(maxy, pts[i].y);
  }
  p_t a{minx, miny};
  p_t b{maxx, maxy};
  return f_t {a, b};
}

topit::Dot::Dot(p_t dd):
 IDraw(),
 d{dd}
{}

