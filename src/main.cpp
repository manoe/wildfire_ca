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


#include <iostream>
#include "wf.h"
#ifdef WF_UT
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("CellPosition constructor value assignment", "[CellPosition]") {
    CellPosition a(2,3);
    REQUIRE( (2==a.x && 3==a.y) );
}

TEST_CASE("CellPosition += operator", "[CellPosition]") {
    CellPosition a(2,3),b(3,4);
    a+=b;
    REQUIRE( (5==a.x && 7==a.y) );
}

TEST_CASE("CellPosition + operator", "[CellPosition]") {
    CellPosition a(2,3),b(3,4);
    CellPosition c=a+b;
    REQUIRE( (5==c.x && 7==c.y) );
    a=a+b;
    REQUIRE( (5==a.x && 7==a.y) );
}

TEST_CASE("CellPosition -= operator", "[CellPosition]") {
    CellPosition a(2,3),b(3,4);
    a-=b;
    REQUIRE( (-1==a.x && -1==a.y) );
}

TEST_CASE("CellPosition - operator", "[CellPosition]") {
    CellPosition a(2,3),b(3,4);
    CellPosition c=a-b;
    REQUIRE( (-1==c.x && -1==c.y) );
    a=a-b;
    REQUIRE( (-1==a.x && -1==a.y) );
}

TEST_CASE("CellPosition *= operator", "[CellPosition]") {
    CellPosition a(2,3);
    int i=8;
    a*=i;
    REQUIRE( (16==a.x && 24==a.y) );
}

TEST_CASE("CellPosition * operator", "[CellPosition]") {
    CellPosition a(2,3);
    int i=8;
    CellPosition b=a*i;
    REQUIRE( (16==b.x && 24==b.y) );
    b=i*a;
    REQUIRE( (16==b.x && 24==b.y) );
}

TEST_CASE("GridCell no-parameter constructor", "[GridCell]") {
    GridCell a;
    REQUIRE( (CellState::NOT_IGNITED==a.state) );
    REQUIRE( (Density::NORMAL==a.den) );
    REQUIRE( (VegetationType::PINE==a.veg) );
    REQUIRE( (0.0f==a.elevation) );
}

TEST_CASE("GridCell parameter constructor", "[GridCell]") {
    GridCell a(CellState::BURNING,
               Density::DENSE,
               VegetationType::THICKETS,
               200.0f);
    REQUIRE( (CellState::BURNING==a.state) );
    REQUIRE( (Density::DENSE==a.den) );
    REQUIRE( (VegetationType::THICKETS==a.veg) );
    REQUIRE( (200.0f==a.elevation) );
}

TEST_CASE("GridCell canBurn() method", "[GridCell]") {
    GridCell a;
    REQUIRE( true==a.canBurn() );
    a.state=CellState::BURNING;
    REQUIRE( false==a.canBurn() );
}



#else
int main() {
    WildFireParams param = { 0.58, 0.045, 0.131, 0.078, 0, 8.1, 100 ,true};
    WildFireCA ca(20,20,param);
    ca.addFireSpot({0,1});
    ca.addFireSpot({4,4});
    CellPosition p_1(1,0);
    std::cout<<3*p_1-p_1<<std::endl;
    ca.step();
    std::cout<<ca<<std::endl;
    ca.step();
    std::cout<<ca<<std::endl;
    return 0;
}
#endif

