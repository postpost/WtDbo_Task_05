#pragma once
#include <wt/dbo/dbo.h>
#include <wt/Dbo/backend/Postgres.h>
#include "shop.h"
#include "book.h"
#include "stock.h"

//сколько книг в одном магазине
class Sale;
class Shop;
class Book;

class Stock {
public:
	int count;
	Wt::Dbo::ptr<Book> book;
	Wt::Dbo::ptr<Shop> shop;
	Wt::Dbo::collection<Wt::Dbo::ptr<Sale>> sale;

	template<typename Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, count, "count");
		Wt::Dbo::belongsTo(a, book, "book");
		Wt::Dbo::belongsTo(a, shop, "shop");
		Wt::Dbo::hasMany(a, sale, Wt::Dbo::ManyToOne, "stock");
	}

};