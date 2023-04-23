/**============================================================================
Name        : ModelViewController.cpp
Created on  : 22.04.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Model View Controller
============================================================================**/

#include "ModelViewController.h"

#include <unordered_map>
#include <string>
#include <iostream>

namespace ModelViewController
{
    class DefectModel {
    public:
        mutable std::unordered_map<std::string, std::string> defects = {
                {"XYZ" , "File doesn't get deleted."},
                {"XAB" , "Registry doesn't get created."},
                {"ABC" , "Wrong title get displayed."}
        };

        std::string getDefectComponent(const std::string& component) const {
            return defects[component];
        }

        size_t getSummary() const {
            return defects.size();
        }

        std::unordered_map<std::string, std::string> getAllDefects() const {
            return defects;
        }
    };

    class DefectView {
    public:
        void showSummary(size_t num) const {
            std::cout << "Their are " + std::to_string(num) + " defects in total!\n";
        }

        void showDefectComponent(const std::string& defect) const {
            std::cout << "Defect of component: " + defect + '\n';
        }

        void showDefectList(const std::unordered_map<std::string, std::string>& defects) const {
            for (const auto& p: defects) {
                std::cout << "(" + p.first + ", " + p.second + ")\n";
            }
        }
    };

    class DefectController
    {
        const DefectModel& defectModel;
        const DefectView& defectView;
    public:
        DefectController(const DefectModel& defModel, const DefectView& defView):
                defectModel{defModel}, defectView{defView} {
        }

        void showDefectComponent(const std::string& component) const {
            defectView.showDefectComponent(defectModel.getDefectComponent(component));
        }

        void showDefectSummary() const {
            defectView.showSummary(defectModel.getSummary());
        }

        void showDefectList() const {
            defectView.showDefectList(defectModel.getAllDefects());
        }
    };
};

void ModelViewController::TestAll()
{
    DefectModel defectModel;
    DefectView defectView;

    DefectController defectController(defectModel, defectView);
    defectController.showDefectComponent("ABC");
    std::cout << '\n';
    defectController.showDefectSummary();
    std::cout << '\n';
    defectController.showDefectList();
};