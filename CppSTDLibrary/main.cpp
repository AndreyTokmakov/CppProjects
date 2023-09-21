
#include <iostream>
#include <memory>
#include <algorithm>
#include <thread>
#include <future>
#include <vector>
#include <fstream>
#include <string>
#include <atomic>

#include "Types.h"
#include "AggregateInitialization/AggregateInitialization.h"
#include "Algorithms/Algorithms.h"
#include "Alignment/Alignment.h"
#include "Integer/Integer.h"
#include "Any/Any.h"
#include "Arrays/Arrays.h"
#include "Asserts/Asserts.h"
#include "Atomic/Atomic.h"
#include "Attributes/Attributes.h"
#include "Auto/Auto.h"
#include "BitSet/BitSet.h"
#include "BitwiseOperation/BitwiseOperation.h"
#include "Byte/Byte.h"
#include "Chrono/Chrono.h"
#include "DateAndTime/DateAndTime.h"
#include "Concepts/Concepts.h"
#include "ConsoleInOut/ConsoleInOut.h"
#include "ConstConstexprMutable/ConstConstexprMutable.h"
#include "CopyElision/CopyElision.h"
#include "Comparators/Comparators.h"
#include "Format/Format.h"
#include "EnumTests/EnumTests.h"
#include "Expected/Expected.h"
#include "Hashing/Hashing.h"
#include "InitializerList/InitializerList.h"
#include "InlineVariables/InlineVariables.h"
#include "Integer/Integer.h"
#include "Types//Int.h"
#include "Initialization/Initialization.h"
#include "FilesStreams/FilesStreams.h"
#include "Filesystem/Filesystem.h"
#include "FunctionObjects/FunctionObjects.h"
#include "IteratorTests/IteratorTests.h"
#include "Lambdas/Lambdas.h"
#include "Locale/Locale.h"
#include "Literals/Literals.h"
#include "Math/Math.h"
#include "PolymorphicMemoryResources/PolymorphicMemoryResources.h"
#include "MoveSemantics_RuleOfFive/MoveSemantics_RuleOfFive.h"
#include "NameSpaces/InlineNamespaces.h"
#include "Numbers/Numbers.h"
#include "NumericLimits/NumericLimits.h"
#include "ObjectOrientedProgramming/ObjectOrientedProgramming.h"
#include "OptionalTests/OptionalTests.h"
#include "Random/Random.h"
#include "Ranges/Ranges.h"
#include "ReferenceWrapper/ReferenceWrapper.h"
#include "Streams/Streams.h"
#include "String/String.h"
#include "Span/Span.h"
#include "StarshipOperator/StarshipOperator.h"
#include "StringStream/StringStreamTests.h"
#include "StringView/StringViewTests.h"
#include "StructuredBinding/StructuredBinding.h"
#include "StrongTypes/StrongTypes.h"
#include "SourceLocation/SourceLocation.h"
#include "Variant/Variant.h"
#include "WeakPtr/WeakPtr.h"
#include "SharedPtr/SharedPtrTests.h"
#include "UniquePtr/UniquePtr.h"
#include "TypeTraits/TypeTraits.h"
#include "Utilities/Utilities.h"
#include "Tuple/TupleTests.h"
#include "TypeCast/TypeCast.h"
#include "Metaprogramming/Metaprogramming.h"
#include "Memory/Memory.h"
#include "Templates/Templates.h"
#include "Volatile/VolatileTests.h"


namespace Utilities {

    constexpr bool DEBUG_PRINT {false};

    class Integer
    {
    private:
        int value {0};

    public:
        Integer(int v): value{v} {
            if constexpr (DEBUG_PRINT) {
                std::cout << "Utilities(" << value << ")\n";
            }
        }

        Integer(const Integer& v): value{v.value} {
            if constexpr (DEBUG_PRINT) {
                std::cout << "Copy Utilities(" << value << ")\n";
            }
        }

        ~Integer() {
            if constexpr (DEBUG_PRINT) {
                std::cout << "~Utilities(" << value << ")\n";
            }
        }

        inline int getValue() const noexcept {
            return value;
        }


        Integer& operator==(const Integer& rhs) = delete;
        Integer& operator==(Integer&& rhs)  = delete;
    };

};


namespace Multithreading {

    void Process_All_CPUs()
    {
        std::vector<std::string> values;
        for (int i = 0; i < 50; ++i) {
            values.push_back(std::string("Value_").append(std::to_string(i)));
        }

        std::atomic<unsigned long> counter {0};
        auto task = [&]()
        {
            size_t id { counter++ };
            while (values.size() > id) {
                const auto& val = values[id];
                std::cout << val << std::endl;
                // std::this_thread::sleep_for(std::chrono::seconds(1));
                id = counter++;
            }
            std::cout << "Thread " << std::this_thread::get_id() << " Done" << std::endl;
        };

        std::vector<std::future<void>> workers;
        const unsigned int threadsCount = std::thread::hardware_concurrency();
        for (unsigned int i = 0; i < threadsCount; ++i) {
            workers.emplace_back(std::async(task));
        }

        std::for_each(workers.cbegin(), workers.cend(), [](const auto& s) { s.wait(); } );
    }


    void Atomic_Tests() {
        std::atomic<unsigned long> counter {3};

        std::cout << counter << std::endl;

        auto result = counter.fetch_add(1, std::memory_order_relaxed);
        std::cout << "result = " << result << std::endl;

        std::cout << counter << std::endl;
    }
}

namespace StdAlgoritms
{
    void Unique() {
        std::vector<int> numbers {1, 2, 2, 3, 4, 3 ,5};
        const auto last = std::unique(numbers.begin(), numbers.end());

        for (auto iter = numbers.cbegin(); last != iter; ++iter)
            std::cout << *iter << " ";
        std::cout << std::endl;
    }
}


namespace Files {

    void ReadFile() {
        constexpr std::string_view filePath {
            R"(/home/andtokm/Projects/CppProjects/CppSTDLibrary/data/test_file.txt)"};

        std::vector<std::string> inputLines, outputLines;
        std::fstream file (filePath.data());

        // Replace "[....]" ---> [....]
        while (std::getline(file, inputLines.emplace_back())) { /** Read file lines **/ }
        for (auto&& s: inputLines) {
            if (auto start = s.find(R"("[)"); std::string::npos != start) {
                if (auto end = s.find(R"(]")", start); std::string::npos != end) {
                    s.erase(s.begin() + start);
                    s.erase(s.begin() + end);
                }
            }
            outputLines.emplace_back(s);
        }

        constexpr std::string_view filePathOut {
                R"(/home/andtokm/Projects/CppProjects/CppSTDLibrary/data/test_file_out.txt)"};
        std::fstream outFile (filePathOut.data(), std::ios::out);
        for (auto s: outputLines)
            outFile << s << '\n';
        outFile.close();
    }
}



int main(int argc, char** argv)
{
    // Types::Tests();
    // Multithreading::Process_All_CPUs();
    // Multithreading::Atomic_Tests();
    // Strings::Parse_Automodeling_StepFileName();
    // StdAlgoritms::Unique();


    // Files::ReadFile();
    // AggregateInitialization::TestAll();
    // Algorithms::TestAll();
    // Alignment::TestAll();
    // Any::TestAll();
    // Arrays::TestAll();
    // Asserts::TestAll();
    // Atomic::TestAll();
    // Attributes::TestAll();
    // Auto::TestAll();
    // BitSet::TestAll();
    // BitwiseOperation::TestAll();
    // Byte::TestAll();
    // Chrono::TestAll();
    // Comparators::TestAll();
    // Concepts::TestAll();
    // ConsoleInOut::TestAll();
    // ConstConstexprMutable::TestAll();
    // CopyElision::TestAll();
    // DateAndTime::TestAll();
    // EnumTests::TestAll();
    // Expected::TestAll();
    // StrongTypes::TestAll();
    // InitializerList::TestAll();
    // InlineVariables::TestAll();
    // Initialization::TestAll();
    // IteratorTests::TestAll();
    // FilesStreams::TestAll();
    // Filesystem::TestAll();
    // Format::TestAll();
    // FunctionObjects::TestAll();
    // Hashing::TestAll();
    // Lambdas::TestAll();
    // Locale::TestAll();
    // Literals::TestAll();
    // MoveSemantics_RuleOfFive::TestAll();
    // Numbers::TestAll();
    // Math::TestAll();
    // Memory::TestAll();
    // Metaprogramming::TestAll();
    // InlineNamespaces::TestAll();
    // NumericLimits::TestAll();
    // ObjectOrientedProgramming::TestAll();
    // Optional::TestAll();
    // PolymorphicMemoryResources::TestAll();
    // Random::TestAll();
    // Ranges::TestAll();
    // ReferenceWrapper::TestAll();
    // StarshipOperator::TestAll();
    // Streams::TestAll();
    // String::TestAll();
    // StringStream::TestAll();
    // StringView::TestAll();
    // StructuredBinding::TestAll();
    // StrongTypes::TestAll();
    // Span::TestAll();
    // SourceLocation::TestAll();
    // SharedPtr_Tests::TestAll();
    // Variant::TestAll();
    // VolatileTests::TestAll();
    // TypeTraits::TestAll();
    // Tuple::TestAll();
    // TypeCast::TestAll();
    // Int::TestAll();
    // Templates::TestAll();
    Utilities::TestAll();
    // UniquePtr_Tests::TestAll();

    // WeakPtr::TestAll();

    return EXIT_SUCCESS;
}