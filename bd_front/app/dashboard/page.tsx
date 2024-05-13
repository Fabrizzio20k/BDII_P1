import QueryTable from "../components/Dashboard";

const dashBoardPage = () => {
    return (
        <div className="h-screen w-screen flex flex-col justify-center bg-gray-800 items-center ">
            <QueryTable />
        </div>
    );
};

export default dashBoardPage;
