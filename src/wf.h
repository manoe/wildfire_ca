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

enum CellState {
    NO_FUEL,
    NOT_IGNITED,
    BURNING,
    BURNED_DOWN
};

enum Density {
    SPARSE  = 0,
    NORMAL  = 1,
    DENSE   = 2
};

enum VegetationType {
    AGRICULTURAL = 0,
    THICKETS     = 1,
    PINE         = 2
};

struct CellPosition {
    int x,y;
    CellPosition(int x, int y) : x(x), y(y) {};
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

    bool canBurn() { return CellState::NOT_IGNITED==state; };
};

struct WildFireParams {
    float p_h,c_1,c_2,a,w_a,w_s,l;
};

class WildFireCA {
    protected:
        GridCell **plane;
        int x_size;
        int y_size;
        WildFireParams params;
        const float p_veg[3] = { -0.3, 0, 0.4 };
        const float p_den[3] = { -0.4, 0, 0.3 };

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

            return std::fabs(f_angle-params.w_a);
        }

        float getSlopeLength(CellPosition from, CellPosition to) {
             if(from.x==to.x || from.y==to.y) {
                 return params.l;
             }
             return std::sqrt(2)*params.l;
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
            return std::exp(params.w_s*(params.c_1+params.c_2*(std::cos(getPropagationWindAngle(from,to))-1.0)));

        }

        float getPs(CellPosition from, CellPosition to) {
            return std::exp(params.a*getSlopeAngle(from,to));
        }

        void propagateFire(CellPosition pos) {
            if(CellState::BURNING != plane[pos.x][pos.y].state) {
                return;
            }

            std::vector<std::pair<CellPosition, GridCell *>> neighbors;

            for(int i:  {-1, 0, 1}) {
                for(int j: {-1, 0, 1}) {
                    if(!(i==0 && j==0) && pos.x+i >= 0 && pos.x+i < x_size && pos.y+j >= 0 && pos.y+j < y_size) {
                        neighbors.push_back(std::pair<CellPosition, GridCell *>(CellPosition(pos.x+i,pos.y+j),&plane[pos.x+i][pos.y+j]));
                    }
                }
            }


            for(auto i : neighbors) {
                float p_burn=params.p_h * (1 + getPveg(i.second->veg)) * (1 + getPden(i.second->den)) * getPw(pos,i.first)*getPs(pos,i.first);
                std::cout<<p_burn<<std::endl;
             }
        }


    public:
        WildFireCA() = delete;
        WildFireCA(int x_size, int y_size, WildFireParams params) : x_size(x_size),
                                                                    y_size(y_size),
                                                                    params(params) {
            plane=new GridCell*[x_size];
            for(int i=0 ; i < x_size ; ++i) {
                plane[i]=new GridCell[y_size];
            }
        };

        ~WildFireCA() {
            for(int i=0 ; i < x_size ; ++i) {
                delete[] plane[i];
            }
            delete[] plane;
        };

        void step() {
            std::cout<<"Step"<<std::endl;
            plane[0][1].state=CellState::BURNING;
            std::vector<CellPosition> cells=collectBurningCells();
            for(auto i : cells) {
                propagateFire(i);
            }
        };
};

#endif /* _WF_H_ */
