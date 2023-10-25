# TransportCatalogue
Реализация транспортного справочника. Получение и сохранение данных в формате JSON с использованием собственной библиотеки. Визуализация карты маршрутов в формате SVG с использованием собственной библиотеки. Поиск кратчайшего пути по заданным условиям на основе построенной карты маршрутов.

# class TransportCatalogue
transport_catalogue.h<br/>
transport_catalogue.cpp<br/>
<br/>
Основные реализованные методы:<br/>
-AddStop(const Stop&& stop) - добавление остановки в справочник<br/>
-AddBus(const Bus&& bus) - добавление автобусного маршрута в справочник<br/>
-FindBus(const std::string_view name) - поиск автобуса по названию<br/>
-FindStop(const std::string_view name) - поиск остановки по названию<br/>
-GetBusesForStop(Stop* stop) - получения списка проходящих через остановку автобусных маршрутов<br/>
-AddDistances(std::pair<std::pair<Stop*, Stop*>, int>&& p) - добавление расстояний между остановками<br/>
-GetDistanceBtwStops(Stop* stop1, Stop* stop2) - получение расстояния между остановками<br/>

# class TransportRouter
transport_router.h<br/>
transport_router.cpp<br/>
Поиск кратчайшего расстояния между остановками

# class JSONReader
json_reader.h<br/>
json_reader.cpp<br/>
Чтение из потока ввода запросов на построение базы данных транспортного справочника, чтение запросов к транспортному справочнику, формирование ответов на полученные запросы в формате JSON

# class MapRenderer
map_renderer.h<br/>
map_renderer.cpp<br/>
Визуализация карты маршрутов в формате SVG
