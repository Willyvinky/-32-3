#pragma once
// ============================================================================
// ВАРИАНТ 32: Анализатор юнит-экономики сервисов совместного потребления
// (Ride-Sharing & Rental Core)
// ============================================================================

#include <string>
#include <cmath>
#include "../../json.hpp"

namespace TaskVariant {

    // ------------------------------------------------------------------------
    // Структура входных данных
    // ------------------------------------------------------------------------
    struct Input {
        double asset_purchase_cost;              // стоимость покупки актива
        double average_trips_per_day;            // среднее количество поездок в день
        double average_trip_duration_mins;       // средняя длительность поездки в минутах
        double per_minute_rental_tariff;         // тариф за минуту аренды
        double daily_maintenance_and_charging_cost; // ежедневные затраты на обслуживание
    };

    // ------------------------------------------------------------------------
    // Структура выходных данных
    // ------------------------------------------------------------------------
    struct Output {
        double daily_gross_revenue;    // дневная валовая выручка
        double net_daily_profit;       // чистая дневная прибыль
        double asset_payback_days;     // срок окупаемости в днях (-1.0, если убыточно)
    };

    // ------------------------------------------------------------------------
    // Проверка входных данных (валидация)
    // ------------------------------------------------------------------------
    inline std::string validate(const Input& in) {
        if (in.asset_purchase_cost <= 0.0)
            return "asset_purchase_cost must be > 0.0";
        if (in.average_trips_per_day <= 0.0)
            return "average_trips_per_day must be > 0.0";
        if (in.average_trip_duration_mins <= 0.0)
            return "average_trip_duration_mins must be > 0.0";
        if ((in.average_trips_per_day * in.average_trip_duration_mins) > 1440.0)
            return "total daily trips duration (trips * duration) must be <= 1440.0";
        if (in.per_minute_rental_tariff < 0.01)
            return "per_minute_rental_tariff must be >= 0.01";
        if (in.daily_maintenance_and_charging_cost <= 0.0)
            return "daily_maintenance_and_charging_cost must be > 0.0";
        return "";
    }

    // ------------------------------------------------------------------------
    // Расчёты по формулам варианта 32
    // ------------------------------------------------------------------------
    inline Output calculate(const Input& in) {
        Output out{};

        // Дневная валовая выручка
        out.daily_gross_revenue = in.average_trips_per_day * in.average_trip_duration_mins * in.per_minute_rental_tariff;

        // Чистая дневная прибыль
        out.net_daily_profit = out.daily_gross_revenue - in.daily_maintenance_and_charging_cost;

        // Срок окупаемости в днях (если прибыль > 0, иначе -1.0)
        if (out.net_daily_profit > 0.0) {
            out.asset_payback_days = in.asset_purchase_cost / out.net_daily_profit;
        } else {
            out.asset_payback_days = -1.0;
        }

        return out;
    }

    // ------------------------------------------------------------------------
    // Разбор JSON-тела запроса в Input
    // ------------------------------------------------------------------------
    inline Input parse(const std::string& body) {
        Input in{};

        auto get_d = [&](const std::string& key) -> double {
            auto p = body.find("\"" + key + "\"");
            if (p == std::string::npos) return 0.0;
            p = body.find(':', p);
            if (p == std::string::npos) return 0.0;
            try { return std::stod(body.substr(p + 1)); }
            catch (...) { return 0.0; }
        };

        in.asset_purchase_cost               = get_d("asset_purchase_cost");
        in.average_trips_per_day             = get_d("average_trips_per_day");
        in.average_trip_duration_mins        = get_d("average_trip_duration_mins");
        in.per_minute_rental_tariff          = get_d("per_minute_rental_tariff");
        in.daily_maintenance_and_charging_cost = get_d("daily_maintenance_and_charging_cost");

        return in;
    }

    // ------------------------------------------------------------------------
    // Формирование JSON-ответа из Output
    // ------------------------------------------------------------------------
    inline std::string to_json(const Output& out) {
        JObj o;
        o.num("daily_gross_revenue", out.daily_gross_revenue);
        o.num("net_daily_profit", out.net_daily_profit);
        o.num("asset_payback_days", out.asset_payback_days);
        return o.str();
    }

} // namespace TaskVariant