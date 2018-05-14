#include <iostream>
#include <gtest/gtest.h>
#include <vector>
#include <tuple>
#include "tuple.h"
#include "checker.h"

using std::vector;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(tuple, construct_default) {
    Checker::events.clear();
    Tuple<Checker, int, Checker> tup;
    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT, CONSTRUCT_DEFAULT};
    EXPECT_EQ(answer, Checker::events);
}

TEST(tuple, destruct) {
    Checker::events.clear();
    auto ptr = new Tuple<Checker>;
    delete ptr;
    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT, DESTRUCT};
    EXPECT_EQ(answer, Checker::events);
}

TEST(tuple, compile) {
    Tuple<int, int, int>(3, 4, 5);
}

TEST(tuple, construct_vars_move) {
    Checker::events.clear();
    Checker c;
    Tuple<int, Checker, Checker> tuple(7, move(c), move(c));

    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT,
                                   CONSTRUCT_MOVE,
                                   CONSTRUCT_MOVE};
    EXPECT_EQ(answer, Checker::events);
}

TEST(tuple, construct_vars_copy) {
    Checker::events.clear();
    Checker c;
    Tuple<Checker, Checker, Checker> tuple(c, move(c), c);

    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT,
                                   CONSTRUCT_COPY,
                                   CONSTRUCT_MOVE,
                                   CONSTRUCT_COPY};
    EXPECT_EQ(answer, Checker::events);
}

TEST(tuple, size) {
    Tuple<int, int, int> tuple;
    EXPECT_EQ(sizeof(tuple), 12);
}

TEST(tuple, copy) {
    Checker::events.clear();
    Tuple<Checker, Checker> tuple;
    Tuple<Checker, Checker> tuple2;
    tuple2 = tuple;

    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT,
                                   CONSTRUCT_DEFAULT,
                                   CONSTRUCT_DEFAULT,
                                   CONSTRUCT_DEFAULT,
                                   COPY,
                                   COPY};
    EXPECT_EQ(answer, Checker::events);
}

TEST(tuple, move) {
    Checker::events.clear();
    Tuple<Checker, Checker> tuple;
    Tuple<Checker, Checker> tuple2;
    tuple2 = move(tuple);

    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT,
                                   CONSTRUCT_DEFAULT,
                                   CONSTRUCT_DEFAULT,
                                   CONSTRUCT_DEFAULT,
                                   MOVE,
                                   MOVE};
    EXPECT_EQ(answer, Checker::events);
}

TEST (tuple, construct_copy) {
    Checker::events.clear();
    Tuple<Checker, Checker> tuple;
    Tuple<Checker, Checker> tuple2(tuple);

    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT,
                                   CONSTRUCT_DEFAULT,
                                   CONSTRUCT_COPY,
                                   CONSTRUCT_COPY};
    EXPECT_EQ(answer, Checker::events);
}

TEST (tuple, construct_move) {
    Checker::events.clear();
    Tuple<Checker, Checker> tuple;
    Tuple<Checker, Checker> tuple2(move(tuple));

    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT,
                                   CONSTRUCT_DEFAULT,
                                   CONSTRUCT_MOVE,
                                   CONSTRUCT_MOVE};
    EXPECT_EQ(answer, Checker::events);
}

TEST (make_tuple, basic) {
    Tuple<int, double> tuple(3, 3.5);
    int x = 5;
    auto tuple2 = makeTuple(x, 5.8);
    tuple = tuple2;
}

TEST (make_tuple, move_and_copy) {
    Checker::events.clear();
    Checker c;
    auto t1 = makeTuple(c, c);
    auto t2 = makeTuple(move(c), move(c));

    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT,
                                   CONSTRUCT_COPY,
                                   CONSTRUCT_COPY,
                                   CONSTRUCT_MOVE,
                                   CONSTRUCT_MOVE};
    EXPECT_EQ(answer, Checker::events);
}

TEST(kth_type, basic) {
    typename KthType<2, int, double, char>::type x = 2.5;
    EXPECT_EQ(x, 2.5);
}

TEST (make_tuple, remove_const) {
    Checker::events.clear();

    const int c = 24;
    auto t = makeTuple(c);
    Tuple<int> t2(t);
}

TEST (get_by_num, basic) {
    auto t = makeTuple(1, 2, 'g');

    EXPECT_EQ(2, get<2>(t));
    get<1>(t) = 10;
    EXPECT_EQ(10, get<1>(t));
}

TEST (get_by_num, move) {
    Checker::events.clear();
    Tuple<Checker, Checker> tuple;
    Checker c(get<2>(move(tuple)));

    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT,
                                   CONSTRUCT_DEFAULT,
                                   CONSTRUCT_MOVE};
    EXPECT_EQ(answer, Checker::events);
}

TEST (get_by_num, copy) {
    Checker::events.clear();
    Tuple<Checker, Checker> tuple;
    Checker c(get<2>(tuple));

    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT,
                                   CONSTRUCT_DEFAULT,
                                   CONSTRUCT_COPY};
    EXPECT_EQ(answer, Checker::events);
}

TEST (get_by_type, basic) {
    auto t = makeTuple(1.0, 2, 'g');

    EXPECT_EQ(2, get<int>(t));
    get<double>(t) = 10.0;
    EXPECT_EQ(10.0, get<double>(t));
}

TEST (get_by_type, move) {
    Checker::events.clear();
    Tuple<int, Checker> tuple;
    Checker c(get<Checker>(move(tuple)));

    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT,
                                   CONSTRUCT_MOVE};
    EXPECT_EQ(answer, Checker::events);
}

TEST (get_by_type, copy) {
    Checker::events.clear();
    Tuple<double, Checker> tuple;
    Checker c(get<Checker>(move(tuple)));

    vector<CheckerEvent> answer = {CONSTRUCT_DEFAULT,
                                   CONSTRUCT_MOVE};
    EXPECT_EQ(answer, Checker::events);
}

TEST (compares, basic) {
    auto t = makeTuple(2, 3, 5);
    auto s = makeTuple(2, 3, 5);
    auto p = makeTuple(2, 4, 5);

    EXPECT_TRUE(t == s);
    EXPECT_FALSE(t == p);

    EXPECT_FALSE(t != s);
    EXPECT_TRUE(t != p);

    EXPECT_FALSE(t < s);
    EXPECT_TRUE(t < p);
    EXPECT_FALSE(p < t);

    EXPECT_TRUE(p > t);
    EXPECT_FALSE(t > p);
    EXPECT_FALSE(t > s);

    EXPECT_TRUE(t <= s);
    EXPECT_TRUE(t <= p);
    EXPECT_FALSE(p <= t);

    EXPECT_TRUE(p >= t);
    EXPECT_FALSE(t >= p);
    EXPECT_TRUE(t >= s);
}

