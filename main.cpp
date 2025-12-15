#include <iostream>
#include <stdexcept>
namespace top {

  struct p_t {
      int x, y;
  };

  struct f_t {
      p_t aa, bb;  
  };
  size_t rows(f_t fr);
  size_t cols(f_t fr);
  bool operator==(p_t a, p_t b);
  bool operator!=(p_t a, p_t b);

  struct IDraw {
    virtual ~IDraw() = default;
    virtual p_t begin() const = 0;
    virtual p_t next(p_t prev) const = 0;
  };
  p_t * extend (const p_t * pts, size_t s, p_t fill);
  void extend(p_t ** pts, size_t & s, p_t fill);
  struct Dot: IDraw {
    explicit Dot(p_t dd);
    p_t begin() const override;
    p_t next(p_t prev) const override;
    p_t d;
  };
  struct Vline : IDraw {
    explicit Vline(p_t start, int lenght);
    p_t begin() const override;
    p_t next(p_t prev) const override;
    p_t start_point;
    int len;
  };
  void append(const IDraw * sh, p_t ** ppts, size_t & s);
  f_t frame(const p_t * pts, size_t s);
  char * canvas(f_t fr, char fill);
  void paint(p_t p, char* cnv, f_t fr, char fill);
  void flush(std::ostream& os, const char * cnv, f_t fr);
}
int main() {
  using namespace top;
  int err = 0;
  IDraw* shp[4] = {};
  p_t * pts = nullptr;
  size_t s = 0;
  try{
    shp [0] = new Dot({0, 0});
    shp [1] = new Dot({2, 3});
    shp [2] = new Dot({-5, -2});
    shp [3] = new Vline({-2, 0}, 5);
      for (size_t i = 0; i < 4; ++i){
	append(shp[i], &pts, s);
      }
      f_t fr = frame(pts, s);
      char * cnv = canvas(fr, '.');
      for (size_t i = 0; i < s; ++i){
        paint(pts[i], cnv, fr, '#');
      }
      flush (std::cout, cnv, fr);
       delete[] cnv;
	} catch (...){
	std::cerr << "Error!\n";
	err = 1;
	}
    delete shp[1];
    delete shp[2];
    delete shp[0];
    delete shp[3];
    return err;
}

void top::paint(p_t p, char * cnv, f_t fr, char fill)
{
	size_t dx = p.x - fr.aa.x;
	size_t dy = fr.bb.y - p.y;
	cnv[dy * cols(fr) + dx] = fill;
}

top::p_t * top::extend (const p_t * pts, size_t s, p_t fill)
{
	p_t * r = new p_t[s+1];
	for (size_t i = 0; i < s; i++){
		r[i] = pts[i];
	}
	r[s] = fill;
	return r;
}

void top::extend(p_t ** pts, size_t & s, p_t fill)
{
	p_t*r = extend(*pts, s, fill);
	delete[] *pts;
	++s;
	*pts = r;
}

void top::append(const IDraw * sh, p_t ** ppts, size_t & s)
{
	extend(ppts, s, sh -> begin());
	p_t b = sh -> begin();
	
	while (sh -> next(b) != sh -> begin()){
		b = sh -> next(b);
		extend(ppts, s, b);
	}
}

void top::flush(std::ostream& os, const char * cnv, f_t fr)
{
	for (size_t i = 0; i < rows(fr); ++i){
		for (size_t j = 0; j < cols(fr); ++j){
			os << cnv[i * cols(fr) + j];
		}
		os << '\n';
	}
}

char * top::canvas(f_t fr, char fill)
{
	size_t s = rows(fr) * cols(fr);
	char * c = new char [s];
	for (size_t i = 0; i < s; ++i){
		c[i] = fill;
	}
	return c;
}

top::f_t top::frame(const p_t * pts, size_t s)
{
	int minx = pts[0].x;
	int miny = pts[0].y;
	int maxx = pts[0].x;
	int maxy = pts[0].y;
	for (size_t i = 0; i < s; ++i){
		minx = std::min(minx, pts[i].x);
		miny = std::min(miny, pts[i].y);
		maxx = std::max(maxx, pts[i].x);
		maxy = std::max(maxy, pts[i].y);
	}
	p_t a{minx, miny};
	p_t b{maxx, maxy};
	return f_t{a, b};
}

top::Dot::Dot(p_t dd):
 IDraw(),
 d{dd}
{}

top::p_t top::Dot::begin() const {
	return d;
}

top::p_t top::Dot::next(p_t prev) const {
	if (prev != d){
		throw std::logic_error("bad prev");
	}
	return d;
}

bool top::operator==(p_t a, p_t b) {
    return a.x == b.x && a.y == b.y;
}

bool top::operator!=(p_t a, p_t b) {
    return !(a == b);
}

size_t top::rows(f_t fr)
{
	return fr.bb.y - fr.aa.y + 1;
}
size_t top::cols(f_t fr)
{
	return fr.bb.x - fr.aa.x + 1;
}

top::Vline::Vline(p_t start, int lenght):
  start_point(start),
  len(lenght) {
  	if (len <= 0){
  		throw std::logic_error("Lenght must be > 0");
  	}
}

top::p_t top::Vline::begin() const {
	return start_point;
}

top::p_t top::Vline::next(p_t prev) const {
    if (prev == start_point) {
        if (len > 1) {
            return {start_point.x, start_point.y + 1};
        }
        return start_point;
    }
    
    int current_index = prev.y - start_point.y;
    if (current_index < len - 1) {
        return {prev.x, prev.y + 1};
    }
    
    return start_point;
}
