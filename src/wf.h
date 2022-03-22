/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2022, Balint Uveges
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _WF_H_
#define _WF_H_

#include <iostream>
#include <vector>
#include <cmath>
#include <random>

#ifndef M_PI
#error "PI constant not available"
#endif

enum CellState {
    NO_FUEL,
    NOT_IGNITED,
    BURNING,
    BURNED_DOWN
};

enum Density {
    EMPTY    = 0,
    SPARSE   = 1,
    NORMAL   = 2,
    DENSE    = 3
};

enum VegetationType {
    NO_VEGETATION = 0,
    AGRICULTURAL  = 1,
    THICKETS      = 2,
    PINE          = 3
};

struct CellPosition {
    int x,y;
    CellPosition(int x, int y) : x(x), y(y) {};
    CellPosition& operator+=(const CellPosition &rhs) {
        this->x+=rhs.x;
        this->y+=rhs.y;
        return *this;
    }

    CellPosition& operator-=(const CellPosition &rhs) {
        this->x-=rhs.x;
        this->y-=rhs.y;
        return *this;
    }

    CellPosition& operator*=(const int i) {
        this->x*=i;
        this->y*=i;
        return *this;
    }

    friend bool operator==(const CellPosition lhs, const CellPosition rhs) {
        return lhs.x==rhs.x && lhs.y==rhs.y;
    }

    friend CellPosition operator+(CellPosition lhs, const CellPosition & rhs) {
        lhs+=rhs;
        return lhs;
    }

    friend CellPosition operator-(CellPosition lhs, const CellPosition &rhs) {
        lhs-=rhs;
        return lhs;
    }

    friend CellPosition operator*(CellPosition lhs, const int & rhs) {
        lhs*=rhs;
        return lhs;
    }

    friend CellPosition operator*(const int & lhs, CellPosition rhs) {
        rhs*=lhs;
        return rhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const CellPosition& pos) {
        os<<pos.x<<" "<<pos.y;
        return os;
   }
};

struct GridCell {
    CellState       state;
    Density         den;
    VegetationType  veg;
    float           elevation;
    GridCell() : GridCell(CellState::NOT_IGNITED,
                          Density::NORMAL,
                          VegetationType::PINE,
                          0.0) {};
    GridCell(CellState state,
             Density den,
             VegetationType veg,
             float elevation) : state(state),
                                den(den),
                                veg(veg),
                                elevation(elevation) {};

    bool canBurn() { return CellState::NOT_IGNITED==state && Density::EMPTY!=den && VegetationType::NO_VEGETATION!=veg; };
};

struct WildFireParams {
    float p_h,  // - p_h - corrective probability coefficent
          c_1,  // - c_1 - wind model's first coefficient - affects speed
          c_2,  // - c_2 - wind model's second coefficient - affects angle
          a,    // - a   - slope model's coefficient
          w_a,  // - w_a - wind angle (North: 0, East: Pi/2, South: Pi, West: 3PI/2), rad
          w_s,  // - w_s - wind speed, m/s
          l;    // - l   - cell's side length, m
    bool  sp=false;   // - sp  - fire spotting enabled/disabled
};

class WildFireCA {
#ifdef WF_UT
    public:
#else
    protected:
#endif
        GridCell **plane;
        int x_size;
        int y_size;
        WildFireParams params;
        const float p_veg[4] = { -1, -0.3, 0, 0.4 };
        const float p_den[4] = { -1, -0.4, 0, 0.3 };
        int counter;

        bool canBurn(CellPosition pos) {
            return validPosition(pos) && plane[pos.x][pos.y].canBurn();
        }

        int getWindSpread() {
            if(params.w_s > 14) {
                return 10;
            }
            if(params.w_s > 12) {
                return 8;
            }
            if(params.w_s > 10) {
                return 6;
            }
            if(params.w_s > 8) {
                return 4;
            }
            return 0;
        }

        std::vector<CellPosition> collectBurningCells() {
            std::vector<CellPosition> burning_cells;
            for(int i=0 ; i < x_size ; ++i) {
                for(int j=0 ; j < y_size ; ++j) {
                    if(CellState::BURNING==plane[i][j].state) {
                        burning_cells.push_back({i,j});
                    }
                }
            }
            return burning_cells;
        }

        void burnDown(CellPosition pos) {
            if(CellState::BURNING==plane[pos.x][pos.y].state) {
                plane[pos.x][pos.y].state=CellState::BURNED_DOWN;
            }
        }

        bool validPosition(CellPosition pos) {
            return pos.x >=0 && pos.y >= 0 && pos.x < x_size && pos.y < y_size;
        }

        float getPropagationWindAngle(CellPosition from, CellPosition to) {
            auto x_dir = to.x-from.x;
            auto y_dir = to.y-from.y;
            float f_angle=0;

            /*
             *      -1       0     +1
             * -1  7/4*PI    0    1/4*PI
             *  0  6/4*PI   N/A   1/2*PI
             * +1  5/4*PI    PI   3/4*PI
             *
             */
            if(x_dir==1) {
                if(y_dir==-1) {
                    f_angle=0.25;
                }
                else if(y_dir==0) {
                    f_angle=0.5;
                }
                else if(y_dir==1) {
                    f_angle=0.75;
                }
            } else if(x_dir==0) {
                if(y_dir==-1) {
                    f_angle=0.0;
                }
                else if(y_dir==0) {
                    /* should throw something */
                    f_angle=10.0;
                }
                else if(y_dir==1) {
                    f_angle=1;
                }
            }
            else if(x_dir==-1) {
                if(y_dir==-1) {
                    f_angle=1.75;
                }
                else if(y_dir==0) {
                    f_angle=1.5;
                }
                else if(y_dir==1) {
                    f_angle=1.25;
                }
            }

            return std::fabs(static_cast<float>(M_PI)*f_angle-params.w_a);
        }

        float getSlopeLength(CellPosition from, CellPosition to) {
             if(from.x==to.x || from.y==to.y) {
                 return params.l;
             }
             return std::sqrt(2.0f)*params.l;
        }
  
        float getSlopeAngle(CellPosition from, CellPosition to) {
            return std::atan((plane[from.x][from.y].elevation-plane[to.x][to.y].elevation)/getSlopeLength(from,to));
        }

        float getPveg(VegetationType veg) {
            return p_veg[veg];
        }

        float getPden(Density den) {
            return p_den[den];
        }

        float getPw(CellPosition from, CellPosition to) {
            return std::exp(params.w_s*(params.c_1+params.c_2*(std::cos(getPropagationWindAngle(from,to))-1.0f)));

        }

        float getPs(CellPosition from, CellPosition to) {
            return std::exp(params.a*getSlopeAngle(from,to));
        }

        void propagateFire(CellPosition pos) {
            if(CellState::BURNING != plane[pos.x][pos.y].state) {
                return;
            }

            std::vector<CellPosition> neighbors;

            for(int i:  {-1, 0, 1}) {
                for(int j: {-1, 0, 1}) {
                    CellPosition n_pos(pos.x+i,pos.y+j);
                    if(!(i==0 && j==0) && validPosition(n_pos) && canBurn(n_pos) ) {
                        neighbors.push_back(n_pos);
                    }
                }
            }
            std::random_device rd;
            std::uniform_real_distribution<float> dist(0,1.0);

            for(auto i : neighbors) {
                float p_burn=params.p_h * (1 + getPveg(plane[i.x][i.y].veg)) * (1 + getPden(plane[i.x][i.y].den)) * getPw(pos,i)*getPs(pos,i);
                if(p_burn>dist(rd)) {
                    plane[i.x][i.y].state=CellState::BURNING;
                    if(params.sp && getWindSpread() > 0 && getPropagationWindAngle(pos,i) < static_cast<float>(M_PI)/10.0f) {
                        auto prop_dir=i-pos;
                        auto spread=getWindSpread();
                        for(int j=2 ; j <= spread ; ++j) {
                            auto s_pos=pos+j*prop_dir;
                            if(validPosition(s_pos)) {
                                plane[s_pos.x][s_pos.y].state=CellState::BURNING;
                            } else {
                                break;
                            }
                        }

                    }
                }
             }
            burnDown(pos);
        }


    public:
        WildFireCA() = delete;
        WildFireCA(int x_size, int y_size, WildFireParams params) : x_size(x_size),
                                                                    y_size(y_size),
                                                                    params(params),
                                                                    counter(0) {
            plane=new GridCell*[x_size];
            for(int i=0 ; i < x_size ; ++i) {
                plane[i]=new GridCell[y_size];
            }
        };

        WildFireCA(int x_size, int y_size, WildFireParams params, GridCell **source_plane) : WildFireCA(x_size, y_size, params) {
            for(int i=0 ; i < x_size ; ++i) {
                for(int j=0 ; j < y_size ; ++j) {
                    plane[i][j] = source_plane[i][j];
                }
            }
        }

        ~WildFireCA() {
            for(int i=0 ; i < x_size ; ++i) {
                delete[] plane[i];
            }
            delete[] plane;
        };

        void addFireSpot(CellPosition pos) {
            if(validPosition(pos)) {
                plane[pos.x][pos.y].state=CellState::BURNING;
            }
        }

        std::vector<CellPosition> stepAndCollect() {
            ++counter;
            std::vector<CellPosition> cells=collectBurningCells();
            for(auto i : cells) {
                propagateFire(i);
            }
            return cells;
        }


        void step() {
            ++counter;
            std::vector<CellPosition> cells=collectBurningCells();
            for(auto i : cells) {
                propagateFire(i);
            }
            
        };

        CellState getState(CellPosition pos) {
            if(!validPosition(pos)) {
               throw std::string("Invalid cell position");
            }
            return plane[pos.x][pos.y].state;
        }

        friend std::ostream& operator<<(std::ostream& os, const WildFireCA& ca) {
            for(int j=0 ; j < ca.y_size ; ++j) {
                for(int i=0 ; i < ca.x_size ; ++i) {
                    os<<ca.plane[i][j].state<<" ";
                }
                os<<std::endl;
            }
            return os;
        };

};

#endif /* _WF_H_ */
