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
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *          and/or other materials provided with the distribution.
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *          AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *          IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *          FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *          DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *          SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *          CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *          OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *          OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _WF_H_
#define _WF_H_

#include <iostream>

enum CellState {
    NO_FUEL,
    NOT_IGNITED,
    BURNING,
    BURNED_DOWN
};

enum Density {
    SPARSE,
    NORMAL,
    DENSE
};

enum VegetationType {
    AGRICULTURAL,
    THICKETS,
    PINE
};

struct GridCell {
    CellState       state;
    Density         den;
    VegetationType  veg;
    float           elevation;
    GridCell() : state(CellState::NOT_IGNITED), den(Density::NORMAL), veg(VegetationType::PINE), elevation(0.0) {};
};

struct WildFireParams {
    float ph,c1,c2,a,w_a,w_s,l;
};

class WildFireCA {
    protected:
        GridCell **plane;
        int x_size;
        int y_size;
        WildFireParams params;

    public:
        WildFireCA() = delete;
        WildFireCA(int x_size, int y_size, WildFireParams params) : x_size(x_size),y_size(y_size),params(params) {
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
        };
};

#endif /* _WF_H_ */
