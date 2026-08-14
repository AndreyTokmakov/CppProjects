/**============================================================================
Name        : main.cpp
Created on  : 29.02.2016
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Cpp STD lib
============================================================================**/

#include "Types.h"
#include "AggregateInitialization/AggregateInitialization.h"
#include "Algorithms/Algorithms.h"
#include "Any/Any.h"
#include "Arrays/Arrays.h"
#include "Asserts/Asserts.h"
#include "Atomic/Atomic.h"
#include "Attributes/Attributes.h"
#include "Auto/Auto.h"
#include "BitSet/BitSet.h"
#include "Bit_Manipulation/BitwiseOperation.hpp"
#include "Byte/Byte.h"
#include "Chrono/Chrono.h"
#include "DateAndTime/DateAndTime.h"
#include "Concepts/Concepts.hpp"
#include "ContextControl/ContextControl.h"
#include "ConsoleInOut/ConsoleInOut.hpp"
#include "CompileTime_Constexpr/ConstConstexprMutable.hpp"
#include "CompileTime_Constexpr/CompileTimeProgramming.hpp"
#include "RVO_CopyElision/CopyElision.h"
#include "RVO_CopyElision/RVO_FailureCases.h"
#include "Comparators/Comparators.h"
#include "CompileTime_Constexpr/Constexpr.hpp"
#include "CTAD/ClassTemplateArgumentDeduction.h"
#include "DeducingThis/DeducingThis.h"
#include "Format/Format.h"
#include "Enums/Enums.hpp"
#include "Embed/BinaryResourceInclusion.hpp"
#include "Expected/Expected.h"
#include "Errors/Errors.h"
#include "Exceptions/Exceptions.h"
#include "Hashing/Hashing.hpp"
#include "InitializerList/InitializerList.h"
#include "InlineVariables/InlineVariables.h"
#include "Types//Int.h"
#include "Initialization/Initialization.h"
#include "FilesStreams/FilesStreams.h"
#include "Filesystem/Filesystem.h"
#include "Functional_Objects/FunctionObjects.hpp"
#include "Iterators/Iterators.h"
#include "Lambdas/Lambdas.h"
#include "LookupTypes/LookupTypes.h"
#include "LifetimeExtension/LifetimeExtension.h"
#include "Locale/Locale.h"
#include "Overflow/Overflow.h"
#include "Literals/Literals.h"
#include "Math/Math.h"
#include "Print/Print.h"
#include "PolymorphicMemoryResources/PolymorphicMemoryResources.h"
#include "Polymorphic/Polymorphic.hpp"
#include "Indirect/Indirect.hpp"
#include "MoveSemantics/MoveSemantics.h"
#include "NameSpaces/InlineNamespaces.h"
#include "Numbers/Numbers.h"
#include "NumericLimits/NumericLimits.h"
#include "ObjectOrientedProgramming/ObjectOrientedProgramming.h"
#include "ObjectOrientedProgramming/RefQualifierMemberFunctions.h"
#include "ObjectOrientedProgramming/ConstexprVirtualFunctions.h"
#include "Optional/Optional.h"
#include "Random/Random.h"
#include "Ranges_Views/Ranges.hpp"
#include "Reflection/Reflection.hpp"
#include "ReferenceWrapper/ReferenceWrapper.h"
#include "Streams/Streams.h"
#include "String/String.h"
#include "Span/Span.h"
#include "SpanStream/SpanStream.hpp"
#include "Simd/Simd.h"
#include "ScopeExit_Experimental/ScopeExit.hpp"
#include "StarshipOperator/StarshipOperator.h"
#include "StringStream/StringStreamTests.h"
#include "StringView/StringViewTests.h"
#include "StructuredBinding/StructuredBinding.h"
#include "StrongTypes/StrongTypes.h"
#include "SourceLocation/SourceLocation.hpp"
#include "Variant/Variant.hpp"
#include "WeakPtr/WeakPtr.h"
#include "SharedPtr/SharedPtrTests.h"
#include "Stacktrace/Stacktrace.h"
#include "UniquePtr/UniquePtr.h"
#include "TypeTraits/TypeTraits.h"
#include "Utilities/Utilities.hpp"
#include "Tuple/TupleTests.h"
#include "TypeCast/TypeCast.h"
#include "Metaprogramming/Metaprogramming.h"
#include "Memory/Memory.h"
#include "Memory/Launder.hpp"
#include "Templates/Templates.hpp"
#include "Volatile/VolatileTests.h"
#include "StaticAsserts/StaticAsserts.h"

#include <vector>
#include <map>
#include <string_view>


int main([[maybe_unused]] const int argc,
         [[maybe_unused]] char** argv,
         [[maybe_unused]] char** environment)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);
    const std::map<std::string_view, std::string_view> env = [&] {
        std::map<std::string_view, std::string_view> envs;
        for (int i = 0; environment && environment[i]; ++i) {
            const std::string_view envVar = environment[i];
            if (const size_t pos = envVar.find('='); std::string::npos != pos) {
                envs.emplace(envVar.substr(0, pos), envVar.substr(pos + 1));
            }
        }
        return envs;
    }();

    // AggregateInitialization::TestAll();
    // Algorithms::TestAll();
    // Any::TestAll();
    // Arrays::TestAll();
    // Asserts::TestAll();
    // Atomic::TestAll();
    // Attributes::TestAll();
    // Auto::TestAll();
    // BitSet::TestAll();
    // Bit_Manipulation::TestAll();
    // bytes::TestAll();
    // Chrono::TestAll();
    // ContextControl::TestAll();
    // Comparators::TestAll();
    // Concepts::TestAll();
    // ConsoleInOut::TestAll();
    // ConstConstexprMutable::TestAll();
    // CompileTimeProgramming::TestAll();
    // Constexpr::TestAll();
    // CopyElision::TestAll();
    // DeducingThis::TestAll();
    // RVO_FailureCases::TestAll();
    // ClassTemplateArgumentDeduction::TestAll();
    // DateAndTime::TestAll();
    // enums::TestAll();
    // Errors::TestAll();
    // Embed::TestAll();
    // Exceptions::TestAll();
    // Expected::TestAll();
    // StrongTypes::TestAll();
    // InitializerList::TestAll();
    // InlineVariables::TestAll();
    // Initialization::TestAll();
    // Iterators::TestAll();
    // FilesStreams::TestAll();
    // Filesystem::TestAll();
    // Format::TestAll();
    // FunctionObjects::TestAll();
    // Hashing::TestAll();
    // LookupTypes::TestAll();
    // Lambdas::TestAll();
    // LifetimeExtension::TestAll();
    // Locale::TestAll();
    // Literals::TestAll();
    // MoveSemantics::TestAll();
    // Numbers::TestAll();
    // Math::TestAll();
    // Memory::TestAll();
    // Metaprogramming::TestAll();
    // InlineNamespaces::TestAll();
    // NumericLimits::TestAll();
    // ObjectOrientedProgramming::TestAll();
    // Optional::TestAll();
    // Overflow::TestAll();
    // PolymorphicMemoryResources::TestAll();
    memory::indirect::TestAll();
    // Print::TestAll();
    // Random::TestAll();
    // Ranges::TestAll();
    // ReferenceWrapper::TestAll();
    // StarshipOperator::TestAll();
    // Streams::TestAll();
    // StringStream::TestAll();
    // String::TestAll();
    // StringView::TestAll();
    // StructuredBinding::TestAll();
    // StrongTypes::TestAll();
    // Span::TestAll();
    // span_stream::TestAll();
    // Simd::TestAll();
    // ScopeExit::TestAll();
    // Stacktrace::TestAll();
    // SourceLocation::TestAll();
    // SharedPtr_Tests::TestAll();
    // Variant::TestAll();
    // VolatileTests::TestAll();
    // TypeTraits::TestAll();
    // Types::Tests();
    // Tuple::TestAll();
    // TypeCast::TestAll();
    // Int::TestAll();
    // Templates::TestAll();
    // Utilities::TestAll();
    // UniquePtr::TestAll();
    // WeakPtr::TestAll();
    // StaticAsserts::TestAll();

    /** C++26 **/

    // Reflection::TestAll();
    // Polymorphic::TestAll();


    return EXIT_SUCCESS;
}
