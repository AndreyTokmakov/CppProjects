cmake_minimum_required(VERSION 3.16)
project(CppSTDLibrary)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME}
        main.cpp
		Types.cpp Types.h
		AggregateInitialization/AggregateInitialization.cpp AggregateInitialization/AggregateInitialization.h
		Algorithms/Algorithms.cpp Algorithms/Algorithms.h
		Any/Any.cpp Any/Any.h
		Arrays/Arrays.cpp Arrays/Arrays.h
		Asserts/Asserts.cpp Asserts/Asserts.h
		Atomic/Atomic.cpp Atomic/Atomic.h
		Attributes/Attributes.cpp Attributes/Attributes.h
		Auto/Auto.cpp Auto/Auto.h
		BitSet/BitSet.cpp BitSet/BitSet.h
		BitwiseOperation/BitwiseOperation.cpp BitwiseOperation/BitwiseOperation.h
		Byte/Byte.cpp Byte/Byte.h
		Chrono/Chrono.cpp Chrono/Chrono.h
		Concepts/Concepts.cpp Concepts/Concepts.h
		ConsoleInOut/ConsoleInOut.cpp ConsoleInOut/ConsoleInOut.h
		DateAndTime/DateAndTime.cpp DateAndTime/DateAndTime.h
		ConstConstexprMutable/ConstConstexprMutable.cpp ConstConstexprMutable/ConstConstexprMutable.h
		CopyElision/CopyElision.cpp CopyElision/CopyElision.h
		EnumTests/EnumTests.cpp EnumTests/EnumTests.h
		Exceptions/Exceptions.cpp Exceptions/Exceptions.h
		Errors/Errors.cpp Errors/Errors.h
		Helpers/UnorderedPair.h Helpers/Helpers.h Helpers/Wrapper.h
		FilesStreams/FilesStreams.cpp FilesStreams/FilesStreams.h
		Filesystem/Filesystem.cpp Filesystem/Filesystem.h
		Format/Format.cpp Format/Format.h
		Format/CustomTypesFormatters.cpp Format/CustomTypesFormatters.h
		FunctionObjects/FunctionObjects.cpp FunctionObjects/FunctionObjects.h
		FunctionObjects/MemberFunctionPointer.cpp FunctionObjects/MemberFunctionPointer.h
		Initialization/Initialization.cpp Initialization/Initialization.h
		InitializerList/InitializerList.cpp InitializerList/InitializerList.h
		InlineVariables/InlineVariables.cpp InlineVariables/InlineVariables.h
		Integer/Integer.cpp Integer/Integer.h
		IteratorTests/IteratorTests.cpp IteratorTests/IteratorTests.h
		Lambdas/Lambdas.cpp Lambdas/Lambdas.h
		LifetimeExtension/LifetimeExtension.cpp LifetimeExtension/LifetimeExtension.h
		Literals/Literals.cpp Literals/Literals.h
		Locale/Locale.cpp Locale/Locale.h
		Math/Math.cpp Math/Math.h
		Memory/Memory.cpp Memory/Memory.h
		Memory/MemoryUsageMonitor.cpp Memory/MemoryUsageMonitor.h
		Memory/Alignment.cpp Memory/Alignment.h
		Memory/AlignedStackAllocator.cpp Memory/AlignedStackAllocator.h
		Memory/CustomStackAllocator.cpp Memory/CustomStackAllocator.h
		MoveSemantics_RuleOfFive/MoveSemantics_RuleOfFive.cpp MoveSemantics_RuleOfFive/MoveSemantics_RuleOfFive.h
		Numbers/Numbers.cpp Numbers/Numbers.h
		NumericLimits/NumericLimits.cpp NumericLimits/NumericLimits.h
		ObjectOrientedProgramming/ObjectOrientedProgramming.cpp ObjectOrientedProgramming/ObjectOrientedProgramming.h
		ObjectOrientedProgramming/Operators.cpp ObjectOrientedProgramming/Operators.h
		OptionalTests/OptionalTests.cpp OptionalTests/OptionalTests.h
		PolymorphicMemoryResources/PolymorphicMemoryResources.cpp PolymorphicMemoryResources/PolymorphicMemoryResources.h
		Random/Random.cpp Random/Random.h
		Ranges/Ranges.cpp Ranges/Ranges.h
		ReferenceWrapper/ReferenceWrapper.cpp ReferenceWrapper/ReferenceWrapper.h
		SharedPtr/SharedPtrTests.cpp SharedPtr/SharedPtrTests.h
		SourceLocation/SourceLocation.cpp SourceLocation/SourceLocation.h
		Span/Span.cpp Span/Span.h
		Streams/Streams.cpp Streams/Streams.h
		String/String.cpp String/String.h
		String/StringUtilities.cpp String/StringUtilities.h
		StringStream/StringStreamTests.cpp StringStream/StringStreamTests.h
		StringView/StringViewTests.cpp StringView/StringViewTests.h
		StructuredBinding/StructuredBinding.cpp StructuredBinding/StructuredBinding.h
		StrongTypes/StrongTypes.cpp StrongTypes/StrongTypes.h
		TestSupport/TestSupport.h
		Tuple/TupleTests.cpp Tuple/TupleTests.h
		TypeCast/TypeCast.cpp TypeCast/TypeCast.h
		TypeTraits/TypeTraits.cpp TypeTraits/TypeTraits.h
		UniquePtr/UniquePtr.cpp UniquePtr/UniquePtr.h
		Utilities/Utilities.cpp Utilities/Utilities.h Utilities/Bind.cpp
		Variant/Variant.cpp Variant/Variant.h
		WeakPtr/WeakPtr.cpp WeakPtr/WeakPtr.h
        Volatile/VolatileTests.cpp Volatile/VolatileTests.h
		Hashing/Hashing.cpp Hashing/Hashing.h
		Metaprogramming/Metaprogramming.cpp Metaprogramming/Metaprogramming.h
		StarshipOperator/StarshipOperator.cpp StarshipOperator/StarshipOperator.h
		Expected/Expected.cpp Expected/Expected.h
		Types/Int.cpp Types/Int.h
		NameSpaces/InlineNamespaces.cpp NameSpaces/InlineNamespaces.h
		Comparators/Comparators.cpp Comparators/Comparators.h
		Templates/Templates.cpp Templates/Templates.h
		Templates/ConditionalExplicit.h Templates/ConditionalExplicit.cpp
		Templates/FoldExpressions.cpp  Templates/FoldExpressions.h
		Templates/NTTP.cpp Templates/NTTP.h
		Templates/TemplateSpecialization.cpp Templates/TemplateSpecialization.h
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
		pthread
		tbb
)