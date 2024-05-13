import QueryResults from "./QueryResults";
import QuerySearch from "./QuerySearch";
import Table from "./TablesInDB";
import Top from "./Top";

const QueryTable = () => {
    return (
        <div className="flex flex-col justify-center align-middle h-4/5 w-7/12 gap-4 ">
            <Top />
            <div className="flex flex-row gap-4 h-2/5">
                <Table />
                <QuerySearch />
            </div>
            <QueryResults />
        </div>
    );
};

export default QueryTable;
