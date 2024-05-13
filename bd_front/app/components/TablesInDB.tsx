import React from "react";

interface TableProps {
    tablename: string;
    numberofelements: number;
}

const Table: React.FC<TableProps> = ({ tablename, numberofelements }) => {
    return (
        <tbody>
            <tr className="bg-white border-b dark:bg-gray-800 dark:border-gray-700 hover:bg-gray-50 dark:hover:bg-gray-600">
                <th
                    scope="row"
                    className="px-6 py-4 font-medium text-gray-900 whitespace-nowrap dark:text-white"
                >
                    {tablename}
                </th>
                <td className="px-6 py-4">{numberofelements}</td>
            </tr>
        </tbody>

        // <div className="flex flex-row bg-white border-b dark:bg-gray-800 dark:border-gray-700 hover:bg-gray-50 dark:hover:bg-gray-600">
        //     <h2>{tablename}</h2>
        //     <p>Number of Elements: {numberofelements}</p>
        // </div>
    );
};

const TableList: React.FC = () => {
    const data = require("./datatable.json");

    return (
        <div className="relative overflow-x-auto overflow-y-auto shadow-md sm:rounded-lg">
            <table className="w-full text-sm text-left rtl:text-right text-gray-500 dark:text-gray-400">
                <thead className="text-xs text-gray-700 uppercase bg-gray-50 dark:bg-gray-700 dark:text-gray-400">
                    <tr>
                        <th scope="col" className="px-6 py-3">
                            Table Name
                        </th>
                        <th scope="col" className="px-6 py-3">
                            Nro of Elements
                        </th>
                    </tr>
                </thead>

                {data.map((table: TableProps, index: number) => (
                    <Table
                        key={index}
                        tablename={table.tablename}
                        numberofelements={table.numberofelements}
                    />
                ))}
            </table>
        </div>
    );
};

export default TableList;
