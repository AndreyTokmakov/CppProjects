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
#include "BitwiseOperation/BitwiseOperation.h"
#include "Byte/Byte.h"
#include "Chrono/Chrono.h"
#include "DateAndTime/DateAndTime.h"
#include "Concepts/Concepts.h"
#include "ContextControl/ContextControl.h"
#include "ConsoleInOut/ConsoleInOut.h"
#include "CompileTime_Constexpr/ConstConstexprMutable.h"
#include "CompileTime_Constexpr/CompileTimeProgramming.h"
#include "RVO_CopyElision/CopyElision.h"
#include "RVO_CopyElision/RVO_FailureCases.h"
#include "Comparators/Comparators.h"
#include "CompileTime_Constexpr/Constexpr.h"
#include "CTAD/ClassTemplateArgumentDeduction.h"
#include "DeducingThis/DeducingThis.h"
#include "Format/Format.h"
#include "EnumTests/EnumTests.h"
#include "Expected/Expected.h"
#include "Errors/Errors.h"
#include "Exceptions/Exceptions.h"
#include "Hashing/Hashing.h"
#include "InitializerList/InitializerList.h"
#include "InlineVariables/InlineVariables.h"
#include "Types//Int.h"
#include "Initialization/Initialization.h"
#include "FilesStreams/FilesStreams.h"
#include "Filesystem/Filesystem.h"
#include "FunctionObjects/FunctionObjects.h"
#include "Iterators/Iterators.h"
#include "Lambdas/Lambdas.h"
#include "LifetimeExtension/LifetimeExtension.h"
#include "Locale/Locale.h"
#include "Overflow/Overflow.h"
#include "Literals/Literals.h"
#include "Math/Math.h"
#include "Print/Print.h"
#include "PolymorphicMemoryResources/PolymorphicMemoryResources.h"
#include "MoveSemantics_RuleOfFive/MoveSemantics_RuleOfFive.h"
#include "NameSpaces/InlineNamespaces.h"
#include "Numbers/Numbers.h"
#include "NumericLimits/NumericLimits.h"
#include "ObjectOrientedProgramming/ObjectOrientedProgramming.h"
#include "ObjectOrientedProgramming/RefQualifierMemberFunctions.h"
#include "ObjectOrientedProgramming/ConstexprVirtualFunctions.h"
#include "Optional/Optional.h"
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
#include "Stacktrace/Stacktrace.h"
#include "UniquePtr/UniquePtr.h"
#include "TypeTraits/TypeTraits.h"
#include "Utilities/Utilities.h"
#include "Utilities/CompillerVersion.h"
#include "Tuple/TupleTests.h"
#include "TypeCast/TypeCast.h"
#include "Metaprogramming/Metaprogramming.h"
#include "Memory/Memory.h"
#include "Templates/Templates.h"
#include "Volatile/VolatileTests.h"
#include "StaticAsserts/StaticAsserts.h"


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // AggregateInitialization::TestAll();
    // Algorithms::TestAll();
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
    // EnumTests::TestAll();
    // Errors::TestAll();
    Exceptions::TestAll();
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
    // Lambdas::TestAll();
    // LifetimeExtension::TestAll();
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
    // Overflow::TestAll();
    // PolymorphicMemoryResources::TestAll();
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
    // Stacktrace::TestAll();
    // SourceLocation::TestAll();
    // SharedPtr_Tests::TestAll();
    // Variant::TestAll();
    // VolatileTests::TestAll();
    TypeTraits::TestAll();
    // Types::Tests();
    // Tuple::TestAll();
    // TypeCast::TestAll();
    // Int::TestAll();
    // Templates::TestAll();
    // Utilities::TestAll();
    // CompilerVersion::TestAll();
    // UniquePtr::TestAll();
    // WeakPtr::TestAll();
    // StaticAsserts::TestAll();

    return EXIT_SUCCESS;
}